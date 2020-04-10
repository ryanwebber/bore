#include <iostream>
#include <string>

#include "runtime.h"
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
 
    auto build_graph = runtime.getBuildGraph();
    auto targets = build_graph->getModules();
    return 0;
}

