#include <iostream>
#include "LuaRuntime.h"

int xlua_invokerule(lua_State* L) {

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushliteral(L, "generate");
    lua_gettable(L, -2);

    if (!lua_isfunction(L, -1))
        luaL_error(L, "rule definition has no generate function");

    // Grab the args from this function
    lua_pushvalue(L, -3);

    // Call the generator function
    if (lua_pcall(L, 1, 1, 0) != 0)
        luaL_error(L, "error running function `f': %s", lua_tostring(L, -1));

    // The return value is already on the stack from the fn call
    return 1;
}

int xlua_defnrule(lua_State* L) {

    /*
     * This looks like this:
     *
     * defnrule = function(name, defn)
     *   if rule exists then
     *     error("Rule already exists")
     *   end
     *
     *   rules[name] = function(args)
     *     return defn.generate(args, context)
     *   end
     *
     *   return rules[name]
     * end
     */

    // Get the rule name
    size_t name_len;
    const char* name = luaL_checklstring(L, 1, &name_len);

    std::cout << "New rule: " << name << std::endl;

    // Make sure the rule doesn't already exist
    // TODO: Can we do this better with metatables?
    if (lua_getglobal(L, name) == LUA_TFUNCTION) {
        luaL_error(L, "global '%s' is already defined - not overwriting", name);
    }

    // Push the rule definition table
    lua_pushvalue(L, 2);

    // Push the closure capturing the rule definition table
    lua_pushcclosure(L, &xlua_invokerule, 1);

    lua_pushvalue(L, -1);
    lua_setglobal(L, name);

    // Return the closure as defnrule retval
    return 1;
}

LuaRuntime::LuaRuntime() {
    L = luaL_newstate();
}

LuaRuntime::~LuaRuntime() {
    lua_close(L);
}

int LuaRuntime::load() {
    luaL_openlibs(L);

    // Add defnrule as a global
    lua_pushcfunction(L, xlua_defnrule);
    lua_setglobal(L, "defnrule");

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

