#include <iostream>
#include "make_generator.h"

#define TAB "\t"

MakeGenerator::MakeGenerator(std::filesystem::path root): root(root) {}

void MakeGenerator::generate(const BuildGraph &graph) {
    for (auto t : graph.getTargets()) {
        for (auto o : t->getRule()->getOutputs()) {
            generateRule(o, t->getRule()->getInputs(), t->getRule()->getCommands());
        }

        // Special case for phony targets
        if (t->getRule()->getOutputs().size() == 0) {
            generateRule(t->getName(), t->getRule()->getInputs(), t->getRule()->getCommands());
        }
    }
}

void MakeGenerator::generateRule(const std::string &name,
                                const std::vector<std::string> &inputs,
                                const std::vector<std::string> &commands) {

    std::cerr << name << ":";
    for (auto i : inputs) {
        std::cerr << " " << i;
    }

    std::cerr << std::endl;

    for (auto c : commands) {
        std::cerr << TAB << c << std::endl;
    }

    std::cerr << std::endl;
}

