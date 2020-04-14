#include <filesystem>
#include "path.h"

namespace fs = std::filesystem;

static int path_join(lua_State *L) {
    int nargs = lua_gettop(L);

    fs::path base;
    for (int i = 0; i < nargs; i++) {
        luaL_argcheck(L, lua_isstring(L, i + 1), i + 1, "Path component should be a string");
        base /= lua_tostring(L, i + 1);
    }

    base.make_preferred();

    lua_pushstring(L, base.c_str());

    return 1;
}

static int path_dirname(lua_State *L) {
    luaL_argcheck(L, lua_isstring(L, 1), 1, "File path should be a string");
    fs::path base(lua_tostring(L, 1));
    fs::path dir = base.parent_path();

    lua_pushstring(L, dir.c_str());

    return 1;
}

static const luaL_Reg libPath [] = {
    { "join",       path_join       },
    { "dirname",    path_dirname    },
    { NULL,         NULL            }
};

int luaopen_path (lua_State *L) {
    lua_newtable(L);

    const luaL_Reg* fn = &libPath[0];
    while (fn->name != NULL) {
        lua_pushcfunction(L, fn->func);
        lua_setfield(L, -2, fn->name);
        fn++;
    }

    lua_setglobal(L, "path");

    return 1;
}

