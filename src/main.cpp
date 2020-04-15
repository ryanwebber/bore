#include <iostream>
#include <string>

#include "runtime.h"
#include "configuration_exception.h"

#include "graph_generator.h"
#include "make_generator.h"

int main(int argc, const char* argv[]) {

    if (argc < 3) {
        std::cerr << "No files provided" << std::endl;
        return 1;
    }

    Runtime runtime;
    std::unique_ptr<BuildGraph> graph;

    try {
        graph = runtime.loadAndEvaluate(argv[1], argv[2]);
    } catch (ConfigurationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
 
    MakeGenerator g("build/deps.gviz");
    g.generate(*graph);

    return 0;
}

