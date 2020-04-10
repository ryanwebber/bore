#include <iostream>
#include <string>

#include "runtime.h"
#include "graph_generator.h"
#include "configuration_exception.h"

int main(int argc, const char* argv[]) {

    if (argc < 3) {
        std::cerr << "No files provided" << std::endl;
        return 1;
    }

    Runtime runtime;
    if (!runtime.load()) {
        return 2;
    }

    try {
        runtime.evaluateBuildScript(argv[1]);

        for (int i = 2; i < argc; i++)
            runtime.evaluateBuildModule(argv[i]);

    } catch (ConfigurationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
 
    GraphGenerator g("build/deps.gviz");

    auto build_graph = runtime.getBuildGraph();
    g.generate(*build_graph);

    return 0;
}

