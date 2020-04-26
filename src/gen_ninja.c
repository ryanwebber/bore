#include <stdio.h>
#include <string.h>

#include "gen_ninja.h"

void ninja_generate(struct BuildGraph *graph,
        struct NinjaOpts *opts,
        struct Error **err) {

    //FILE *f = fopen(opts->ninja_file, "w");
    //if (f == NULL) {
    //    return error_fmt(err, "Unable to open %s", opts->makefile);
    //}

    FILE *f = stderr;

    struct TargetList *tlist = graph->list;
    while (tlist != NULL) {
        struct List *outs = &tlist->target->rule->outputs;
        struct List *ins = &tlist->target->rule->inputs;
        struct List *cmds = &tlist->target->rule->commands;

        if (!list_empty(outs) && !list_empty(cmds)) {
            fprintf(f, "rule r__%s\n", tlist->target->name);
            fprintf(f, "\tcommand = ");

            struct ListNode *cnode = list_first(cmds);
            while (cnode != NULL) {
                if (cnode != list_first(cmds)) {
                    fprintf(f, " && \\\n\t\t");
                }

                fprintf(f, "%s", cnode->value);
                cnode = list_next(cnode);
            }

            fprintf(f, "\n\n");

            fprintf(f, "build");
            struct ListNode *onode = list_first(outs);
            while (onode != NULL) {
                fprintf(f, " %s", onode->value);
                onode = list_next(onode);
            }

            fprintf(f, ": r__%s", tlist->target->name);

            struct ListNode *inode = list_first(ins);
            while (inode != NULL) {
                fprintf(f, " %s", inode->value);
                inode = list_next(inode);
            }

            fprintf(f, "\n\n");
        }

        tlist = tlist->next;
    }
}

