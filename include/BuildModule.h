#ifndef BUILDMODULE_H
#define BUILDMODULE_H

#include <map>
#include <memory>
#include <string>

#include "Target.h"

class BuildModule {
    private:
        std::string name;
        std::string modulePath;
        std::map<std::string, std::shared_ptr<Target>> targets;

    public:
        BuildModule(std::string name, std::string modulePath);
        ~BuildModule() = default;

        std::string getName();
        std::string getModulePath();
};

#endif

