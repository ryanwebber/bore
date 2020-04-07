#include <iostream>
#include "LuaRuntime.h"

LuaRuntime::LuaRuntime() {
    L = luaL_newstate();
}

LuaRuntime::~LuaRuntime() {
    lua_close(L);
}

int LuaRuntime::load() {
    luaL_openlibs(L);
    return 0;
}

int LuaRuntime::evaluateScript(const std::string& script) {
    int status = luaL_dostring(L, script.c_str());
    if (status) {
        std::cout << lua_tostring(L, -1) << std::endl;
    }

    return status;
}

int LuaRuntime::evaluateFile(const std::string& filename) {
    int status = luaL_dofile(L, filename.c_str());
    if (status) {
        std::cout << lua_tostring(L, -1) << std::endl;
    }

    return status;
}

