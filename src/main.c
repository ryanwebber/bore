#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>

int main() {

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    int status = luaL_dostring(L, "print(\"Hello world!\")");
    if (status) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);
    return status;
}

