#ifndef MAKE_GENERATOR_H
#define MAKE_GENERATOR_H

#include "build_graph.h"

struct MakeOpts {
    const char* makefile;
};

void make_generate(const BuildGraph &graph, const MakeOpts &opts);

#endif

