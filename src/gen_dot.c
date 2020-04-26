#include <stdio.h>
#include "gen_dot.h"

#define TAB "    "

void dot_generate(struct BuildGraph *graph) {
    FILE *g = stderr;

    fprintf(g, "digraph D {\n");

    struct TargetList *t = graph->list;
    while (t != NULL) {
        fprintf(g, "%s\"%s\";\n", TAB, t->target->name);
        t = t->next;
    }

    fprintf(g, "\n");

    t = graph->list;
    while (t != NULL) {
        struct ListNode *ins = list_first(&t->target->rule->inputs);
        while (ins != NULL) {
            struct Target *producer = graph_dep_search(graph, ins->value);
            if (producer != NULL) {
                fprintf(g, "%s\"%s\" -> \"%s\";\n", TAB, t->target->name, producer->name);
            }

            ins = list_next(ins);
        }

        t = t->next;
    }

    fprintf(g, "}\n");
}

