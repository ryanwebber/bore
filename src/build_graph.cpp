#include <sstream>
#include "build_graph.h"
#include "configuration_exception.h"

void BuildGraph::addTarget(const std::shared_ptr<Target> target) {
    if (targets.find(target->getName()) != targets.end()) {
        throw ConfigurationException("Target '" + target->getName() + "' is already defined");
    }

    // TODO check for cyclic dependencies here
    for (auto output : target->getRule()->getOutputs()) {
        auto existing_target = findTargetProducing(output);
        if (existing_target != NULL) {
            throw ConfigurationException("Targets '" + target->getName() + "' and '" +
                    existing_target->getName() + "' both produce output '" + output + "'");
        }

        dependencies[output] = target;
    }

    targets[target->getName()] = target;
}

std::vector<std::shared_ptr<Target>> BuildGraph::getTargets() const {
    std::vector<std::shared_ptr<Target>> v;
    for (auto target : targets) {
        v.push_back(target.second);
    }

    return v;
}

std::shared_ptr<Target> BuildGraph::findTarget(const std::string &target) const {
    if (targets.find(target) == targets.end()) {
        return NULL;
    }

    return targets.at(target);
}

std::shared_ptr<Target> BuildGraph::findTargetProducing(const std::string &output) const {
    if (dependencies.find(output) == dependencies.end()) {
        return NULL;
    }

    return dependencies.at(output);
}

