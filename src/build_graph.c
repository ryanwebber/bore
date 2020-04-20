#include <stdlib.h>
#include "build_graph.h"

void graph_init(struct BuildGraph *graph) {
    graph->targets = malloc(sizeof(struct TSTree));
    tstree_init(graph->targets);

    graph->deps = malloc(sizeof(struct TSTree));
    tstree_init(graph->deps);

    graph->list = NULL;
}

void graph_free(struct BuildGraph *graph) {
    tstree_free(graph->targets);
    free(graph->targets);

    tstree_free(graph->deps);
    free(graph->deps);

    struct TargetList *ln = graph->list;
    while (ln != NULL) {
        struct TargetList *to_free = ln;
        ln = ln->next;

        target_free(to_free->target);
        free(to_free->target);
        free(to_free);
    }
}

void graph_insert_target(struct BuildGraph *graph, struct Target *target, struct Error **err) {
    struct Target * existing_target = (struct Target*) tstree_get_or_insert(graph->targets,
            target->name, target);

    if (existing_target != NULL) {
        return error_fmt(err, "Target '%s' is already defined", target->name);
    }

    struct TargetList *ln = malloc(sizeof(struct TargetList));
    ln->target = target;
    ln->next = graph->list;
    graph->list = ln;

    struct ListNode *n = list_first(&target->rule->outputs);
    while (n != NULL) {
        struct Target *colliding_target = (struct Target*) tstree_get_or_insert(graph->deps,
                n->value, target);

        if (colliding_target != NULL) {
            return error_fmt(err, "Targets '%s' and '%s' both produce '%s'",
                    target->name, colliding_target->name, n->value);
        }

        n = list_next(n);
    }
}

struct Target* graph_get_target(struct BuildGraph *graph, const char *name) {
    return (struct Target*) tstree_lookup(graph->targets, name);
}

struct Target* graph_dep_search(struct BuildGraph *graph, const char *output) {
    return (struct Target*) tstree_lookup(graph->deps, output);
}

