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

