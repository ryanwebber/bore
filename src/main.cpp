#include <iostream>
#include <string>

#include "Runtime.h"

int main(int argc, const char* argv[]) {

    if (argc < 3) {
        std::cerr << "No files provided" << std::endl;
        return 1;
    }

    Runtime runtime;
    if (!runtime.load()) {
        return 2;
    }

    if (!runtime.evaluateBuildScript(argv[1])) {
        return 3;
    }

    if (!runtime.evaluateBuildModule(argv[2])) {
        return 4;
    }

    auto build_graph = runtime.getBuildGraph();
    auto targets = build_graph->getModules();
    return 0;
}

