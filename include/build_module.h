#ifndef BUILD_MODULE_H
#define BUILD_MODULE_H

#include <map>
#include <memory>
#include <filesystem>
#include <string>

#include "target.h"

class BuildModule {
    private:
        std::string name;
        std::filesystem::path modulepath;
        std::map<std::string, std::shared_ptr<Target>> targets;

    public:
        BuildModule(std::string name, std::string modulepath);
        ~BuildModule() = default;

        std::string getName();
        std::filesystem::path getModulePath();
};

#endif

