#ifndef GRAPH_GENERATOR_H
#define GRAPH_GENERATOR_H

#include <filesystem>
#include "build_module.h"
#include "build_graph.h"
#include "generator.h"

class GraphGenerator: public Generator {
    private:
        std::filesystem::path outfile;

    public:
        GraphGenerator(std::filesystem::path outfile);
        ~GraphGenerator() = default;

        void generate(const BuildGraph &graph);
};

#endif
