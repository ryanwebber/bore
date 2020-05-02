#ifndef LUA_RUNTIME_H
#define LUA_RUNTIME_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "build_graph.h"
#include "keyvalues.h"
#include "error.h"


struct LuaRuntime {
    lua_State *L;
};

void runtime_init(struct LuaRuntime *runtime);
void runtime_free(struct LuaRuntime *runtime);

void runtime_evaluate(
        struct LuaRuntime *runtime,
        const char* build_file,
        const char* build_dir,
        struct KeyValueList *config,
        struct BuildGraph *graph,
        struct Error **err);

#endif

