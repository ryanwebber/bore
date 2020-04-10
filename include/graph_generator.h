#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include "build_module.h"
#include "build_graph.h"
#include "generator.h"

class GraphGenerator: public Generator {
    public:
        GraphGenerator() = default;
        ~GraphGenerator() = default;

        bool generateBuildFiles(const BuildModule &module, const BuildGraph &graph);
};

#endif
