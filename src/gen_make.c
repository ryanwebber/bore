#include <stdio.h>
#include <string.h>

#include "error.h"
#include "gen_make.h"
#include "list.h"
#include "strset.h"

#define TAB "\t"

void emit_make_rule(
        FILE *m,
        const char *output,
        struct List *inputs,
        struct List *dirs,
        struct List *commands) {

    fprintf(m, "%s:", output);

    for (struct ListNode *node = list_first(inputs); node != NULL; node = list_next(node)) {
        fprintf(m, " %s", node->value);
    }

    if (!list_empty(dirs)) {
        fprintf(m, " |");
        for (struct ListNode *node = list_first(dirs); node != NULL; node = list_next(node)) {
            fprintf(m, " %s", node->value);
        }
    }

    fprintf(m, "\n");

    for (struct ListNode *node = list_first(commands); node != NULL; node = list_next(node)) {
        fprintf(m, "%s%s\n", TAB, node->value);
    }

    fprintf(m, "\n");
}

void make_generate(struct BuildGraph *graph, struct MakeOpts *opts, struct Error **err) {

    FILE *m = fopen(opts->makefile, "w");
    if (m == NULL) {
        return error_fmt(err, "Unable to open %s", opts->makefile);
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

    // Fourth: Emit the all target if needed
    if (!list_empty(defaults.values)) {
        emit_make_rule(m, "all", defaults.values, &empty, &empty);
        sset_insert(&phonys, "all");
    }

    // Fifth: Emit the normal targets
    tlist = graph->list;
    while (tlist != NULL) {
        const char *name = tlist->target->name;
        struct List* tdirs = &tlist->target->rule->dirs;
        struct List* tins = &tlist->target->rule->inputs;
        struct List* touts = &tlist->target->rule->outputs;
        struct List* tcmds = &tlist->target->rule->commands;

        // If we're expecting to create a phony target, create it now
        if (sset_has(&phonys, name)) {
            if (tlist->target->phony || list_empty(touts)) {
                // If we're a true phony, include commands we might have
                emit_make_rule(m, name, tins, &empty, tcmds);
            } else {
                // If it's not a true phony, it's an alias so ignore the commands
                emit_make_rule(m, name, touts, &empty, &empty);
            }
        }

        struct ListNode *out = list_first(touts);
        while (out != NULL) {
            emit_make_rule(m, out->value, tins, tdirs, tcmds);
            out = list_next(out);
        }

        tlist = tlist->next;
    }

    // Sixth: Emit the directory targets
    dir = list_first(dirs.values);
    while (dir != NULL) {
        fprintf(m, "%s:\n", dir->value);
        fprintf(m, "%s@mkdir -p $@\n\n", TAB);
        dir = list_next(dir);
    }

    // Seventh: Emit the .PHONY target
    if (!list_empty(phonys.values)) {
        fprintf(m, ".PHONY:");

        struct ListNode *phony = list_first(phonys.values);
        while(phony != NULL) {
            fprintf(m, " %s", phony->value);
            phony = list_next(phony);
        }

        fprintf(m, "\n\n");
    }

    sset_free(&defaults);
    sset_free(&phonys);
    sset_free(&dirs);
    list_free(&empty);
}

