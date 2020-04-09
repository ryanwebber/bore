
#include "BuildModule.h"

BuildModule::BuildModule(std::string name, std::string modulePath):
        name(name),
        modulePath(modulePath),
        targets(std::map<std::string, std::shared_ptr<Target>>()) {}

std::string BuildModule::getName() {
    return name;
}

std::string BuildModule::getModulePath() {
    return modulePath;
}

