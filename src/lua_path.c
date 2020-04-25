#include <assert.h>

#include "lua_path.h"
#include "path.h"

static int lua_path_join(lua_State *L) {
    int nargs = lua_gettop(L);
    assert(nargs >= 0);

    size_t expected_len = 0;
    const char* parts[nargs + 1];
    parts[nargs] = NULL;

    for (int i = 0; i < nargs; i++) {
        luaL_argcheck(L, lua_isstring(L, i + 1), i + 1, "Path component should be a string");

        size_t len;
        parts[i] = lua_tolstring(L, i + 1, &len);

        // Spare an arbitrary amount of bytes for path separators
        expected_len += len + 8;
    }

    char buf[expected_len];
    size_t pathlen = path_join_many(parts, buf, expected_len);

    lua_pushlstring(L, buf, pathlen);
    return 1;
}

static int lua_path_dirname(lua_State *L) {
    luaL_argcheck(L, lua_isstring(L, 1), 1, "File path should be a string");

    const char* p = lua_tostring(L, 1);
    const char* dir;
    size_t len = path_dirname(p, &dir);

    lua_pushlstring(L, dir, len);
    return 1;
}

static int lua_path_basename(lua_State *L) {
    luaL_argcheck(L, lua_isstring(L, 1), 1, "File path should be a string");

    const char* p = lua_tostring(L, 1);
    const char* name;
    size_t len = path_basename(p, &name);

    lua_pushlstring(L, name, len);
    return 1;
}

static int lua_path_filename(lua_State *L) {
    luaL_argcheck(L, lua_isstring(L, 1), 1, "File path should be a string");

    const char* p = lua_tostring(L, 1);
    const char* name;
    size_t len = path_filename(p, &name);

    lua_pushlstring(L, name, len);
    return 1;
}

static int lua_path_extension(lua_State *L) {
    luaL_argcheck(L, lua_isstring(L, 1), 1, "File path should be a string");

    const char* p = lua_tostring(L, 1);
    const char* ext;
    size_t len = path_extension(p, &ext);

    lua_pushlstring(L, ext, len);
    return 1;
}

static const luaL_Reg libPath [] = {
    { "join",       lua_path_join       },
    { "dirname",    lua_path_dirname    },
    { "basename",   lua_path_basename   },
    { "filename",   lua_path_filename   },
    { "extension",  lua_path_extension  },
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

