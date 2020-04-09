
#include "BuildGraph.h"

bool BuildGraph::hasModule(const std::string &name) {
    return modules.find(name) != modules.end();
}

bool BuildGraph::insertModule(const std::shared_ptr<BuildModule> module) {
    if (hasModule(module->getName())) {
        return false;
    }

    modules[module->getName()] = module;

    return NULL;
}

std::shared_ptr<BuildModule> BuildGraph::lookupModule(const std::string &name) {
    if (hasModule(name)) {
        return modules[name];
    } else {
        return NULL;
    }
}

