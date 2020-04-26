#ifndef GEN_MAKE_H
#define GEN_MAKE_H

#include "build_graph.h"

struct MakeOpts {
    const char* makefile;
};

void make_generate(struct BuildGraph *graph, struct MakeOpts *opts, struct Error **err);

#endif

