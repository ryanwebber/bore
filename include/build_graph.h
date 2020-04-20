#ifndef BUILD_GRAPH_H
#define BUILD_GRAPH_H

#include "error.h"
#include "list.h"
#include "target.h"
#include "tstree.h"

struct TargetList {
    struct Target *target;
    struct TargetList *next;
};

struct BuildGraph {
    struct TargetList *list;
    struct TSTree *targets;
    struct TSTree *deps;
};

void graph_init(struct BuildGraph *graph);
void graph_free(struct BuildGraph *graph);

void graph_insert_target(struct BuildGraph *graph, struct Target *target, struct Error **err);

struct Target* graph_get_target(struct BuildGraph *graph, const char* name);
struct Target* graph_dep_search(struct BuildGraph *graph, const char* output);

#endif

