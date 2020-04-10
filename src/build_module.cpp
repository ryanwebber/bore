#include <cassert>
#include "build_module.h"

namespace fs = std::filesystem;

BuildModule::BuildModule(std::string name, std::string buildfilepath): name(name) {
    this->buildfilepath = fs::path(buildfilepath);
    this->modulepath = this->buildfilepath.parent_path();
}

std::string BuildModule::getName() const {
    return name;
}

fs::path BuildModule::getModulePath() const {
    return modulepath;
}

fs::path BuildModule::getBuildFilePath() const {
    return buildfilepath;
}

std::vector<std::shared_ptr<Target>> BuildModule::getTargets() const {
    std::vector<std::shared_ptr<Target>> v;
    for (auto i : targets) {
        v.push_back(i.second);
    }

    return v;
}

std::shared_ptr<Target> BuildModule::findTarget(std::string &name) const {
    if (!hasTarget(name)) {
        return NULL;
    }

    return targets.at(name);
}

bool BuildModule::hasTarget(std::string &name) const {
    return targets.find(name) != targets.end();
}

bool BuildModule::addTarget(std::shared_ptr<Target> target) {
    std::string target_name = target->getName();
    assert(hasTarget(target_name) == false);
    targets[target_name] = target;
    return true;
}

