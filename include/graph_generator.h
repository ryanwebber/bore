#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <filesystem>

#include "generator.h"
#include "build_graph.h"

class GraphGenerator: public Generator {
    public:
        GraphGenerator() = default;
        ~GraphGenerator() = default;

        void generate(const BuildGraph &graph, ArgOpts &opts);
};

#endif

