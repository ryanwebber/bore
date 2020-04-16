#ifndef RUNTIME_H
#define RUNTIME_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "build_graph.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

struct RuntimeConfiguration {
    const std::string build_dir;
};

class Runtime {

    private:
        lua_State *L;
        std::unique_ptr<BuildGraph> graph;

        void loadLibs();
        void loadGlobals();
        void extractRule();

    public:
        Runtime();
        Runtime(const Runtime& other) = delete;
        ~Runtime();

        std::unique_ptr<BuildGraph> loadAndEvaluate(
                                        const std::string &buildpath,
                                        const RuntimeConfiguration &conf);
};

#endif

