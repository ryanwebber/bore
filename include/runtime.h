#ifndef RUNTIME_H
#define RUNTIME_H

#include <memory>
#include <string>
#include <vector>

#include "build_graph.h"

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

class Runtime {

    private:
        lua_State *L;
        std::unique_ptr<BuildGraph> graph;

        void loadLibs();
        void loadGlobals();
        void extractRule();

    public:
        Runtime();
        ~Runtime();

        // Disable the copy constructor, or we'll mess up ownership
        // of the lua_State
        Runtime(const Runtime& other) = delete;

        std::unique_ptr<BuildGraph> loadAndEvaluate(
                const std::vector<std::string> &filepaths);
};

#endif

