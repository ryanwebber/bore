#ifndef PATH_H
#define PATH_H

extern "C" {
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
}

int luaopen_path(lua_State *L);

#endif

