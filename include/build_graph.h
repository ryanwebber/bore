#ifndef BUILD_GRAPH_H
#define BUILD_GRAPH_H

#include <map>
#include <memory>

#include "build_module.h"

class BuildGraph {
    private:
        std::map<std::string, std::shared_ptr<BuildModule>> modules;

    public:
        BuildGraph() = default;
        ~BuildGraph() = default;

        std::shared_ptr<BuildModule> lookupModule(const std::string &name) const;
        bool hasModule(const std::string &name) const;
        bool insertModule(const std::shared_ptr<BuildModule> module);
};

#endif
