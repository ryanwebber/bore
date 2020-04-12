#include <iostream>
#include "graph_generator.h"

#define TAB "    "

GraphGenerator::GraphGenerator(std::filesystem::path outfile): outfile(outfile) {}

void GraphGenerator::generate(const BuildGraph &graph) {
    std::cerr << "digraph D {" << std::endl;

    auto modules = graph.getModules();
    for (auto module : modules) {
        std::cerr << TAB << "subgraph cluster_" << module->getName() << " {" << std::endl;
        std::cerr << TAB TAB << "label = \"" << module->getName() << "\";" << std::endl;
        std::cerr << TAB TAB << "color = blue;" << std::endl;
        std::cerr << std::endl;

        for (auto target : module->getTargets()) {
            std::cerr << TAB TAB
                << module->getName()
                << "_"
                << target->getName()
                << " [label=\""
                << target->getName()
                << "\"];"
                << std::endl;
        }

        std::cerr << TAB << "}" << std::endl << std::endl;
    }

    for (auto module : modules) {
        for (auto target : module->getTargets()) {
            auto rule = target->getRule();
            for (auto input : rule->getInputs()) {
                auto target_producer = graph.findProducerOf(input);
                if (target_producer != NULL) {
                    std::cerr << TAB
                        << module->getName()
                        << "_"
                        << target->getName()
                        << " -> "
                        << target_producer->getModuleName()
                        << "_"
                        << target_producer->getName()
                        << ";"
                        << std::endl;
                }
            }
        }
    }

    std::cerr << "}" << std::endl;
}

