
#include "build_graph.h"

std::shared_ptr<BuildModule> BuildGraph::lookupModule(const std::string &name) const {
    if (hasModule(name)) {
        return modules.at(name);
    } else {
        return NULL;
    }
}

std::vector<std::shared_ptr<BuildModule>> BuildGraph::getModules() const {
    std::vector<std::shared_ptr<BuildModule>> v;
    for (auto &i : modules) {
        v.push_back(i.second);
    }

    return v;
}

bool BuildGraph::hasModule(const std::string &name) const {
    return modules.find(name) != modules.end();
}

bool BuildGraph::insertModule(const std::shared_ptr<BuildModule> module) {
    if (hasModule(module->getName())) {
        return false;
    }

    modules[module->getName()] = module;

    return true;
}

