#include <iostream>
#include <sstream>
#include <set>

#include "make_generator.h"
#include "file_writer.h"
#include "generation_exception.h"

#define TAB "\t"

void MakeGenerator::generate(const BuildGraph &graph, ArgOpts &opts) {

    auto makefile = opts.get("--make-output");
    FileWriter fw(makefile);

    std::set<std::string> phonies;
    std::set<std::string> dirs;

    auto all = graph.findTarget("all");
    if (all != NULL) {
        generateRule(fw, all->getName(), all->getRule()->getInputs(), {}, {});
        *fw << std::endl;
    }

    // First pass, create rules and collect dir listings
    for (auto t : graph.getTargets()) {
        auto target_dirs = t->getRule()->getDirs();
        for (auto o : t->getRule()->getOutputs()) {
            generateRule(fw, o, t->getRule()->getInputs(), target_dirs, t->getRule()->getCommands());
            *fw << std::endl;
        }

        for (auto d : target_dirs) {
            auto colliding_target = graph.findTargetProducing(d);
            if (colliding_target != NULL) {
                std::stringstream ss;
                ss << "Directory dependency '"
                   << d
                   << "' in target '"
                   << t->getName()
                   << "' collides with build rule in target '"
                   << colliding_target->getName()
                   << "'";

                throw GenerationException(ss.str());
            }

            dirs.insert(d);
        }
    }

    // Second pass, try to make phony targets
    for (auto t : graph.getTargets()) {
        if (t->getRule()->getOutputs().empty()) {
            auto name = t->getName();
            if (dirs.find(name) != dirs.end()) {
                std::stringstream ss;
                ss << "Directory dependency '"
                   << name
                   << "' collides with phony build rule with the same name (in target '"
                   << name
                   << "')";

                throw GenerationException(ss.str());
            }

            phonies.insert(name);

            if (all == NULL || all->getName() != name) {
                generateRule(fw, name, t->getRule()->getInputs(), {}, t->getRule()->getCommands());
                *fw << std::endl;
            }
        }
    }

    for (auto d : dirs) {
        *fw << d << ":" << std::endl << TAB << "@mkdir -p $@" << std::endl << std::endl;
    }

    if (phonies.size() > 0) {
        *fw << ".PHONY:";
        for (auto p : phonies) {
            *fw << " " << p;
        }

        *fw << std::endl << std::endl;
    }
}

void MakeGenerator::generateRule(FileWriter &fw,
                                 const std::string &name,
                                 const std::vector<std::string> &inputs,
                                 const std::vector<std::string> &dirs,
                                 const std::vector<std::string> &commands) {

    *fw << name << ":";
    for (auto i : inputs) {
        *fw << " " << i;
    }

    if (!dirs.empty()) {
        *fw << " |";
        for (auto d : dirs) {
            *fw << " " << d;
        }
    }

    *fw << std::endl;

    for (auto c : commands) {
        *fw << TAB << c << std::endl;
    }
}

