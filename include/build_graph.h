#ifndef BUILD_GRAPH_H
#define BUILD_GRAPH_H

#include <map>
#include <memory>
#include <vector>

#include "target.h"

class BuildGraph {
    private:
        std::map<std::string, std::shared_ptr<Target>> targets;
        std::map<std::string, std::shared_ptr<Target>> dependencies;

    public:
        BuildGraph() = default;
        ~BuildGraph() = default;

        void addTarget(const std::shared_ptr<Target> target);
        std::vector<std::shared_ptr<Target>> getTargets() const;
        std::shared_ptr<Target> findTarget(const std::string &target) const;
        std::shared_ptr<Target> findTargetProducing(const std::string &file) const;
};

#endif

