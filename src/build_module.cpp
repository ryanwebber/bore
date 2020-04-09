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

std::shared_ptr<Target> BuildModule::findTarget(std::string &name) const {
    if (targets.find(name) == targets.end()) {
        return NULL;
    }

    return targets.at(name);
}

bool BuildModule::addTarget(std::shared_ptr<Target> target) {
    std::string target_name = target->getName();
    if (findTarget(target_name) != NULL) {
        return false;
    }

    targets[target_name] = target;
    return true;
}

