#ifndef RUNTIME_H
#define RUNTIME_H

#include <memory>
#include <string>

#include "BuildGraph.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class Runtime {

    private:
        lua_State* L;
        std::shared_ptr<BuildGraph> graph;

    public:
        Runtime();
        ~Runtime();

        Runtime(const Runtime& other) = delete;

        bool load();
        bool evaluateBuildModule(const std::string& filepath);
        bool evaluateBuildScript(const std::string& filepath);
};

#endif

