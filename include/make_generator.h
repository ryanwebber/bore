#ifndef MAKE_GENERATOR_H
#define MAKE_GENERATOR_H

#include "build_graph.h"

struct MakeOpts {
    const char* makefile;
};

void make_generate(struct BuildGraph *graph, struct MakeOpts *opts, struct Error **err);

#endif

