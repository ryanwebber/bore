#include <iostream>
#include "Runtime.h"

Runtime::Runtime() {
    L = luaL_newstate();
    graph = std::make_shared<BuildGraph>();
}

Runtime::~Runtime() {
    lua_close(L);
}

bool Runtime::load() {
    luaL_openlibs(L);
    return true;
}

bool Runtime::evaluateBuildModule(const std::string& filepath) {

    // Load the file into a chunk  and calling, expecing one return value
    // (the module)
    int err = luaL_loadfile(L, filepath.c_str()) || lua_pcall(L, 0, 1, 0);

    if (err) {
        std::cout << lua_tostring(L, -1) << std::endl;
        return err;
    }

    if (!lua_istable(L, -1)) {
        std::cout << "Expected build file to return a module: "
            << filepath << std::endl;
    }

    lua_getfield(L, -1, "name");

    size_t len;
    const char* mod_name = lua_tolstring(L, -1, &len);

    if (mod_name == NULL) {
        std::cerr << "Module must define a name: " << filepath << std::endl;
        return false;
    }

    if (len == 0) {
        std::cerr << "Module name cannot be empty: " << filepath << std::endl;
        return false;
    }

    return !err;
}

bool Runtime::evaluateBuildScript(const std::string& filepath) {
    int status = luaL_dofile(L, filepath.c_str());
    if (status) {
        std::cout << lua_tostring(L, -1) << std::endl;
        return false;
    }

    // Clear the stack before returning
    lua_settop(L, 0);

    return true;
}

