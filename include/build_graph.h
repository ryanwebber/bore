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

        std::vector<std::shared_ptr<BuildModule>> getModules() const;
        std::shared_ptr<BuildModule> findModule(const std::string &name) const;
        bool hasModule(const std::string &name) const;
        void addModule(const std::shared_ptr<BuildModule> module);
};

#endif

