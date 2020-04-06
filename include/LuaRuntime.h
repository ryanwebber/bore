#ifndef LUARUNTIME_H
#define LUARUNTIME_H

#include <string>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class LuaRuntime {

    private:
        lua_State* L;

    public:
        LuaRuntime();
        ~LuaRuntime();
        
        LuaRuntime(const LuaRuntime& other) = delete;

        int load();
        int evaluateScript(const std::string& script);
        int evaluateFile(const std::string& filename);
};

#endif

