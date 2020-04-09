#include "build_module.h"

namespace fs = std::filesystem;

BuildModule::BuildModule(std::string name, std::string modulepath):
        name(name),
        modulepath(modulepath),
        targets(std::map<std::string, std::shared_ptr<Target>>()) {}

std::string BuildModule::getName() {
    return name;
}

fs::path BuildModule::getModulePath() {
    return modulepath;
}

