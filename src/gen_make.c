#include <stdio.h>
#include <string.h>

#include "error.h"
#include "gen_make.h"
#include "list.h"
#include "strset.h"

#define TAB "\t"

void generateRule(
        FILE *m,
        const char *name,
        struct List *inputs,
        struct List *deps,
        struct List *dirs,
        struct List *commands) {

    fprintf(m, "%s:", name);

    for (struct ListNode *node = list_first(inputs); node != NULL; node = list_next(node)) {
        fprintf(m, " %s", node->value);
    }

    for (struct ListNode *node = list_first(deps); node != NULL; node = list_next(node)) {
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
}

void make_generate(struct BuildGraph *graph, struct MakeOpts *opts, struct Error **err) {

    FILE *m = fopen(opts->makefile, "w");
    if (m == NULL) {
        return error_fmt(err, "Unable to open %s", opts->makefile);
    }

    struct TSTree vtargets;
    tstree_init(&vtargets);

    struct StringSet defaults;
    sset_init(&defaults);

    struct StringSet phonies;
    sset_init(&phonies);

    struct StringSet dirs;
    sset_init(&dirs);

    struct List empty;
    list_init(&empty);

    struct TargetList *tlist = graph->list;
    while (tlist != NULL) {
        struct ListNode *dep = list_first(&tlist->target->rule->deps);
        while (dep != NULL) {
            tstree_insert(&vtargets, dep->value, NULL);
            dep = list_next(dep);
        }

        if (tlist->target->primary) {
            if (tlist->target->phony) {
                sset_insert(&defaults, tlist->target->name);
            } else {
                struct ListNode *tout = list_first(&tlist->target->rule->outputs);
                while (tout != NULL) {
                    sset_insert(&defaults, tout->value);
                    tout = list_next(tout);
                }
            }
        }

        tlist = tlist->next;
    }

    if (!list_empty(defaults.values)) {
        generateRule(m, "all", defaults.values, &empty, &empty, &empty);
        fprintf(m, "\n");
        sset_insert(&phonies, "all");
    }

    // First pass, create rules and collect dir listings
    tlist = graph->list;
    while (tlist != NULL) {
        struct List* tdirs = &tlist->target->rule->dirs;
        struct List* tins = &tlist->target->rule->inputs;
        struct List* tdeps = &tlist->target->rule->deps;
        struct List* tcmds = &tlist->target->rule->commands;

        struct ListNode *tout = list_first(&tlist->target->rule->outputs);
        while (tout != NULL) {
            generateRule(m, tout->value, tins, tdeps, tdirs, tcmds);
            fprintf(m, "\n");

            tout = list_next(tout);
        }

        struct ListNode *tdir = list_first(tdirs);
        while (tdir != NULL) {
            struct Target *collision = graph_dep_search(graph, tdir->value);
            if (collision != NULL) {
                return error_fmt(err, 
                        "Directory '%s' created in target '%s' collides with build rule in target'%s'",
                        tdir->value, tlist->target->name, collision->name);
            }

            sset_insert(&dirs, tdir->value);
            tdir = list_next(tdir);
        }

        tlist = tlist->next;
    }

    // Second pass, try to make phony targets
    tlist = graph->list;
    while (tlist != NULL) {
        const char *name = tlist->target->name;
        if (tlist->target->phony) {

            if (sset_has(&dirs, name)) {
                return error_fmt(err, "Directory '%s' collides with a phony build rule", name);
            }

            sset_insert(&phonies, name);

            struct List* tins = &tlist->target->rule->inputs;
            struct List* tdeps = &tlist->target->rule->deps;
            struct List* tcmds = &tlist->target->rule->commands;
            generateRule(m, name, tins, tdeps, &empty, tcmds);
            fprintf(m, "\n");
        } else if (tstree_test(&vtargets, tlist->target->name)) {
            generateRule(m, name, &tlist->target->rule->outputs, &empty, &empty, &empty);
            fprintf(m, "\n");
        }

        tlist = tlist->next;
    }

    struct ListNode *dir = list_first(dirs.values);
    while (dir != NULL) {
        fprintf(m, "%s:\n", dir->value);
        fprintf(m, "%s@mkdir -p $@\n\n", TAB);
        dir = list_next(dir);
    }

    if (!list_empty(phonies.values)) {
        fprintf(m, ".PHONY:");

        struct ListNode *phony = list_first(phonies.values);
        while(phony != NULL) {
            fprintf(m, " %s", phony->value);
            phony = list_next(phony);
        }

        fprintf(m, "\n\n");
    }

    sset_free(&phonies);
    sset_free(&dirs);
    list_free(&empty);
    tstree_free(&vtargets);
}
