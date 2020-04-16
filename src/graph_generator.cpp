#include <iostream>
#include "graph_generator.h"

#define TAB "    "

void GraphGenerator::generate(const BuildGraph &graph, ArgOpts &opts) {
    std::cerr << "digraph D {" << std::endl;

    for (auto target : graph.getTargets()) {
        std::cerr << TAB << target->getName() << ";" << std::endl;
    }

    std::cerr << std::endl;

    for (auto target : graph.getTargets()) {
        auto rule = target->getRule();
        for (auto input : rule->getInputs()) {
            auto target_producer = graph.findTargetProducing(input);
            if (target_producer != NULL) {
                std::cerr << TAB
                    << target->getName()
                    << " -> "
                    << target_producer->getName()
                    << ";"
                    << std::endl;
            }
        }
    }

    std::cerr << "}" << std::endl;
}

