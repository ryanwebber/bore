#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <filesystem>

#include "generator.h"
#include "build_graph.h"

class GraphGenerator: public Generator {
    private:
        std::filesystem::path root;

    public:
        GraphGenerator(std::filesystem::path root);
        ~GraphGenerator() = default;

        void generate(const BuildGraph &graph);
};

#endif

