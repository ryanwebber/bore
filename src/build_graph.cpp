#include <sstream>
#include "build_graph.h"
#include "configuration_exception.h"
#include "duplicate_module_exception.h"

std::shared_ptr<BuildModule> BuildGraph::findModule(const std::string &name) const {
    if (!hasModule(name)) {
        return NULL;
    }

    return modules.at(name);
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

void BuildGraph::addModule(const std::shared_ptr<BuildModule> module) {
    if (hasModule(module->getName())) {
        auto existing_module = findModule(module->getName());
        throw DuplicateModuleException(module, existing_module);
    }

    for (auto target : module->getTargets()) {
        for (auto rule : target->getRules()) {
            for (auto output : rule->getOutputs()) {
                if (output_deps.find(output) != output_deps.end()) {
                    std::stringstream ss;
                    ss << "Multiple rules produce output '"
                        << output
                        << "' (in targets '"
                        << target->getQualifiedName()
                        << "' and '"
                        << output_deps.at(output)->getQualifiedName()
                        << "')"
                        << std::endl;

                    throw ConfigurationException(ss.str());
                }

                output_deps[output] = target;
            }
        }
    }

    modules[module->getName()] = module;
}

