#include <iostream>
#include "graph_generator.h"

bool GraphGenerator::generateBuildFiles(const BuildModule &module, const BuildGraph &graph) {
    std::cerr << "Generating files for: " << module.getBuildFilePath() << std::endl;
    return true;
}

