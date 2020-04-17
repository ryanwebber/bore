#include <iostream>
#include <set>

#include "make_generator.h"
#include "file_writer.h"
#include "generation_exception.h"

#define TAB "\t"

void MakeGenerator::generate(const BuildGraph &graph, ArgOpts &opts) {

    auto makefile = opts.get("--make-output");
    FileWriter fw(makefile);

    std::set<std::string> phonies;

    auto all = graph.findTarget("all");
    if (all != NULL) {
        generateRule(fw, all->getName(), all->getRule()->getInputs(), {});
        *fw << std::endl;
    }

    for (auto t : graph.getTargets()) {
        for (auto o : t->getRule()->getOutputs()) {
            generateRule(fw, o, t->getRule()->getInputs(), t->getRule()->getCommands());
            *fw << std::endl;
        }
    }

    if (phonies.size() > 0) {
        *fw << ".PHONY:";
        for (auto p : phonies) {
            *fw << " " << p;
        }

        *fw << std::endl;
    }
}

void MakeGenerator::generateRule(FileWriter &fw,
                                 const std::string &name,
                                 const std::vector<std::string> &inputs,
                                 const std::vector<std::string> &commands) {

    *fw << name << ":";
    for (auto i : inputs) {
        *fw << " " << i;
    }

    *fw << std::endl;

    for (auto c : commands) {
        *fw << TAB << c << std::endl;
    }
}

