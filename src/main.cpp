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
    std::unique_ptr<BuildGraph> graph;
    std::vector<std::string> files {
        argv[1], argv[2]
    };

    try {
        graph = runtime.loadAndEvaluate(files);
    } catch (ConfigurationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
 
    GraphGenerator g("build/deps.gviz");
    g.generate(*graph);

    return 0;
}

