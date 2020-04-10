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
        std::filesystem::path buildfilepath;
        std::map<std::string, std::shared_ptr<Target>> targets;

    public:
        BuildModule(std::string name, std::string buildfilepath);
        ~BuildModule() = default;

        std::string getName() const;
        std::vector<std::shared_ptr<Target>> getTargets() const;
        std::filesystem::path getModulePath() const;
        std::filesystem::path getBuildFilePath() const;

        std::shared_ptr<Target> findTarget(std::string &name) const;
        bool hasTarget(std::string &name) const;
        bool addTarget(std::shared_ptr<Target> target);
};

#endif

