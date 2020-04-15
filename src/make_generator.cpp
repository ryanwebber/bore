#include <iostream>
#include <set>
#include "make_generator.h"

#define TAB "\t"

MakeGenerator::MakeGenerator(std::filesystem::path root): root(root) {}

void MakeGenerator::generate(const BuildGraph &graph) {

    std::set<std::string> phonies;

    for (auto t : graph.getTargets()) {
        for (auto o : t->getRule()->getOutputs()) {
            generateRule(o, t->getRule()->getInputs(), t->getRule()->getCommands());
        }

        // If there are no output file collisions with the target name,
        // then add a phony target for it
        if (graph.findTargetProducing(t->getName()) == NULL && t->getRule()->getInputs().size() > 0) {
            std::vector<std::string> empty;
            generateRule(t->getName(), t->getRule()->getInputs(), empty);
            phonies.insert(t->getName());
        }
    }

    if (phonies.size() > 0) {
        std::cerr << ".PHONY:";
        for (auto p : phonies) {
            std::cerr << " " << p;
        }

        std::cerr << std::endl;
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

