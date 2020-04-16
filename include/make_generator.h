#ifndef MAKE_GENERATOR_H
#define MAKE_GENERATOR_H

#include <filesystem>

#include "generator.h"
#include "build_graph.h"

class MakeGenerator: public Generator {
    private:
        void generateRule(const std::string &name,
                          const std::vector<std::string> &inputs,
                          const std::vector<std::string> &commands);

    public:
        MakeGenerator() = default;
        ~MakeGenerator() = default;

        void generate(const BuildGraph &graph);
};

#endif

