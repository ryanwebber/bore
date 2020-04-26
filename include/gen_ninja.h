#ifndef NINJA_GENERATOR_H
#define NINJA_GENERATOR_H

#include "build_graph.h"

struct NinjaOpts {
    const char* ninja_file;
};

void ninja_generate(struct BuildGraph *graph, struct NinjaOpts *opts, struct Error **err);

#endif

