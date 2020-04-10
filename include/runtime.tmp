#ifndef RUNTIME_H
#define RUNTIME_H

#include <memory>
#include <string>

#include "build_graph.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class Runtime {

    private:
        lua_State* L;
        std::shared_ptr<BuildGraph> graph;

        void extractTargets(BuildModule& module);
        void extractRules(Target& target, BuildModule& module);

    public:
        Runtime();
        ~Runtime();

        Runtime(const Runtime& other) = delete;

        bool load();
        void evaluateBuildModule(const std::string& filepath);
        void evaluateBuildScript(const std::string& filepath);

        std::shared_ptr<BuildGraph> getBuildGraph() const;
};

#endif

