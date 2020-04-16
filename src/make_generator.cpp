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

    for (auto t : graph.getTargets()) {
        for (auto o : t->getRule()->getOutputs()) {
            generateRule(fw, o, t->getRule()->getInputs(), t->getRule()->getCommands());
        }

        // If there are no output file collisions with the target name,
        // then add a phony target for it
        if (graph.findTargetProducing(t->getName()) == NULL && t->getRule()->getInputs().size() > 0) {
            generateRule(fw, t->getName(), t->getRule()->getInputs(), {});
            phonies.insert(t->getName());
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

    *fw << std::endl;
}

