#include <cassert>
#include <iostream>
#include <filesystem>

#include "runtime.h"
#include "configuration_exception.h"

namespace fs = std::filesystem;

static const char kBuildGraphRegistryMarker = 0x55;

static int submodule(lua_State *L) {
    std::cerr << "Submodule called" << std::endl;

    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_gettable(L, LUA_REGISTRYINDEX);
    // BuildGraph *graph = (BuildGraph*) lua_touserdata(L, -1);

    return 0;
}

static int target(lua_State *L) {
    std::cerr << "Target created" << std::endl;
    return 0;
}

Runtime::Runtime() {
    L = luaL_newstate();
    graph = std::make_unique<BuildGraph>();
}

Runtime::~Runtime() {
    lua_close(L);
}

void Runtime::loadLibs() {
    luaL_openlibs(L);
}

void Runtime::loadGlobals() {
    // the global submodule function
    lua_pushcfunction(L, submodule);
    lua_setglobal(L, "submodule");

    // the global target function
    lua_pushcfunction(L, target);
    lua_setglobal(L, "target");

    // push the build graph into the regustry
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_pushlightuserdata(L, (void *) graph.get());
    lua_settable(L, LUA_REGISTRYINDEX);
    std::cerr << "Graph pointer " << graph.get() << std::endl;
}

std::unique_ptr<BuildGraph> Runtime::loadAndEvaluate(const std::vector<std::string> &filepaths) {
    // loadAndEvalueate can only be called once because it taints
    // the lua runtime
    assert(graph != nullptr);

    loadLibs();
    loadGlobals();

    for (auto file : filepaths) {
        int err = luaL_dofile(L, file.c_str());
        if (err) {
            throw ConfigurationException(lua_tostring(L, -1));
        }
    }

    return std::move(graph);
}

void Runtime::extractRule() {

    // TODO: Replace 'rule' rule with userdata, and check metatable
    // here
    lua_getfield(L, -1, "ins");
    lua_getfield(L, -2, "outs");
    lua_getfield(L, -3, "cmds");
    if (!lua_istable(L, -1)) {
        throw ConfigurationException("TMP ERROR");
    }

    auto rule = std::make_shared<Rule>();

    // Adding the commands
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            throw ConfigurationException("TMP ERROR");
        }

        rule->addCommand(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // Pop back to outputs
    lua_pop(L, 1);

    // Adding the outputs
    if (lua_isstring(L, -1)) {
        rule->addOutput(lua_tostring(L, -1));
    } else if (lua_istable(L, -1) && lua_rawlen(L, -1) > 0) {
        lua_pushnil(L);  /* first key */
        while (lua_next(L, -2) != 0) {
            if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                throw ConfigurationException("TMP ERROR");
            }

            rule->addOutput(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    // Pop back to inputs
    lua_pop(L, 1);

    // Adding the inputs
    if (lua_isstring(L, -1)) {
        rule->addOutput(lua_tostring(L, -1));
    } else if (lua_istable(L, -1) && lua_rawlen(L, -1) > 0) {
        lua_pushnil(L);  /* first key */
        while (lua_next(L, -2) != 0) {
            if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                throw ConfigurationException("TMP ERROR");
            }

            rule->addInput(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }

    // Remember to do this one last time
    lua_pop(L, 1);
}

