#include <stdio.h>
#include <string.h>

#include "error.h"
#include "gen_ninja.h"
#include "list.h"
#include "strset.h"

void emit_ninja_list(FILE *m, struct List *l) {
    struct ListNode *node = list_first(l);
    while (node != NULL) {
        fprintf(m, " %s", node->value);
        node = list_next(node);
    }
}

void emit_ninja_rule_name(FILE *m, const char *rule) {
    size_t len = strlen(rule);
    char buf[len + 1];

    for (size_t i = 0; i < len; i++) {
        if ((rule[i] >= 'A' && rule[i] <= 'Z') ||
                (rule[i] >= 'a' && rule[i] <= 'z') ||
                (rule[i] >= '0' && rule[i] <= '9')) {
            buf[i] = rule[i];
        } else {
            buf[i] = '_';
        }
    }

    buf[len] = '\0';
    fprintf(m, "r__%s", buf);
}

void emit_ninja_rule(FILE *m, const char *name, struct List *commands) {
    fprintf(m, "rule ");
    emit_ninja_rule_name(m, name);
    fprintf(m, "\n  command =");

    struct ListNode *cmd = list_first(commands);
    while (cmd != NULL) {
        if (cmd != list_first(commands)) {
            fprintf(m, " &&");
        }

        fprintf(m, " %s", cmd->value);
        cmd = list_next(cmd);
    }

    fprintf(m, "\n");
}

void ninja_generate(struct BuildGraph *graph, struct NinjaOpts *opts, struct Error **err) {

    FILE *m = fopen(opts->ninja_file, "w");
    if (m == NULL) {
        return error_fmt(err, "Unable to open %s", opts->ninja_file);
    }

    struct StringSet defaults;
    sset_init(&defaults);

    struct StringSet phonys;
    sset_init(&phonys);

    struct StringSet dirs;
    sset_init(&dirs);

    struct List empty;
    list_init(&empty);

    // First: collect phonys, dirs, and default targets
    struct TargetList *tlist = graph->list;
    while (tlist != NULL) {
        const char *name = tlist->target->name;

        // Make a note of the target if it's maked as default
        if (tlist->target->primary) {
            sset_insert(&defaults, name);
        }

        // Create a phony target if it is listed as phony, of if the target name
        // is not already an output in the project
        if (tlist->target->phony ||
                (graph_dep_search(graph, name) == NULL && tlist->target->alias)) {

            sset_insert(&phonys, name);
        }

        // Keep track of all the dirs we will ever need
        struct ListNode *dir = list_first(&tlist->target->rule->dirs);
        while (dir != NULL) {
            sset_insert(&dirs, dir->value);
            dir = list_next(dir);
        }

        tlist = tlist->next;
    }

    // Second: Check phonys for collisions with dirs and outputs
    struct ListNode *phony = list_first(phonys.values);
    while (phony != NULL) {
        const char *name = phony->value;

        if (sset_has(&dirs, name)) {
            return error_fmt(err, "Directory '%s' collides with phony target '%s'",
                    name, name);
        }

        struct Target *tcoll = graph_dep_search(graph, name);
        if (tcoll != NULL) {
            return error_fmt(err, "Target '%s' output collides with phony target '%s'",
                    tcoll->name, name);
        }

        phony = list_next(phony);
    }

    // Third: Check dirs for collisions against outputs
    struct ListNode *dir = list_first(dirs.values);
    while (dir != NULL) {
        const char *name = dir->value;

        struct Target *tcoll = graph_dep_search(graph, name);
        if (tcoll != NULL) {
            return error_fmt(err, "Target '%s' output collides with directory '%s'",
                    tcoll->name, name);
        }

        dir = list_next(dir);
    }

    // Fourth: Emit the normal targets
    tlist = graph->list;
    while (tlist != NULL) {
        const char *name = tlist->target->name;
        struct List* tdirs = &tlist->target->rule->dirs;
        struct List* tins = &tlist->target->rule->inputs;
        struct List* touts = &tlist->target->rule->outputs;
        struct List* tcmds = &tlist->target->rule->commands;

        if (tlist->target->phony && !list_empty(tcmds)) {
            // If the true phony has commands, it needs a rule
            emit_ninja_rule(m, name, tcmds);
            fprintf(m, "\n");
            fprintf(m, "build %s: ", name);
            emit_ninja_rule_name(m, name);
            emit_ninja_list(m, tins);
            fprintf(m, "\n\n");
        } else if (tlist->target->phony) {
            // If it's a true phony with no commands,
            // we skip the rule part and use the ninja phony rule
            fprintf(m, "build %s: phony", name);
            emit_ninja_list(m, tins);
            fprintf(m, "\n\n");

        } else if (sset_has(&phonys, name)) {
            // If it's a virtual phony then we use the ninja phony rule
            fprintf(m, "build %s: phony", name);
            emit_ninja_list(m, touts);
            fprintf(m, "\n\n");
        }

        if (!tlist->target->phony) {
            emit_ninja_rule(m, name, tcmds);
            fprintf(m, "\n");
            fprintf(m, "build");
            emit_ninja_list(m, touts);
            fprintf(m, ": ");
            emit_ninja_rule_name(m, name);
            emit_ninja_list(m, tins);

            if (!list_empty(tdirs)) {
                fprintf(m, " |");
                emit_ninja_list(m, tdirs);
            }

            fprintf(m, "\n\n");
        }

        tlist = tlist->next;
    }

    // Fifth: Emit the directory targets
    fprintf(m, "rule mkdir\n  command = mkdir -p $out\n");
    dir = list_first(dirs.values);
    while (dir != NULL) {
        fprintf(m, "\nbuild %s: mkdir", dir->value);
        dir = list_next(dir);
    }

    fprintf(m, "\n\n");

    // Sixth: Emit the default target if needed
    if (!list_empty(defaults.values)) {
        struct ListNode *def = list_first(defaults.values);
        while (def != NULL) {
            fprintf(m, "default %s\n", def->value);
            def = list_next(def);
        }

        fprintf(m, "\n");
    }

    sset_free(&defaults);
    sset_free(&phonys);
    sset_free(&dirs);
    list_free(&empty);
}

