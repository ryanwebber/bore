#include <cassert>
#include <iostream>
#include <filesystem>

#include "runtime.h"
#include "path.h"
#include "fglob.h"
#include "configuration_exception.h"

static const char *kRuleMetatableMarker = "Bore.RuleMarker";
static const char *kBuildGraphRegistryMarker = "__usedAsAddress";

namespace fs = std::filesystem;

static int rule(lua_State *L) {
    Rule **prule = reinterpret_cast<Rule**>(lua_newuserdata(L, sizeof(Rule*)));
    *prule = new Rule();

    luaL_getmetatable(L, kRuleMetatableMarker);
    lua_setmetatable(L, -2);

    // Parse out the rules, assuming the lua core has already
    // done some basic data formatting and validation for us
    Rule &rule = **prule;

    int s_top_start = lua_gettop(L);

    lua_getfield(L, 1, "ins");
    lua_getfield(L, 1, "outs");
    lua_getfield(L, 1, "cmds");

    // Adding the commands
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            luaL_argerror(L, 1, "Expected commands to be a list of strings");
        }

        const char* cmd = lua_tostring(L, -1);
        rule.addCommand(cmd);
        lua_pop(L, 1);
    }

    // Pop back to outputs
    lua_pop(L, 1);

    // Adding the outputs
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            luaL_argerror(L, 1, "Expected outputs to be a list of strings");
        }

        const char* output = lua_tostring(L, -1);
        rule.addOutput(output);
        lua_pop(L, 1);
    }

    // Pop back to inputs
    lua_pop(L, 1);

    // Adding the inputs
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            luaL_argerror(L, 1, "Expected inputs to be a list of strings");
        }

        const char* input = lua_tostring(L, -1);
        rule.addInput(input);
        lua_pop(L, 1);
    }

    // Remember to do this one last time
    lua_pop(L, 1);

    // General sanity check. The stack gets emptied when we
    // return here anyways
    int s_top_end = lua_gettop(L);
    assert(s_top_start == s_top_end);

    // The rule userdatum is now on the top of the stack
    return 1;
}

static Rule* rule_check(lua_State *L, int index) {
    Rule *rule;
    luaL_checktype(L, index, LUA_TUSERDATA);

    void* udata = luaL_testudata(L, index, kRuleMetatableMarker);
    if (udata == NULL)
        return NULL;

    rule = *reinterpret_cast<Rule**>(udata);
    return rule;
}

static int rule_gc(lua_State *L) {
    Rule *rule = rule_check(L, -1);
    luaL_argcheck(L, rule != NULL, 1, "Unexpected non-rule type received");
    delete rule;
    return 0;
}

static int submodule(lua_State *L) {
    const char* path = lua_tostring(L, 1);
    if (luaL_loadfile(L, path)) {
        lua_error(L);
        return 0;
    }

    lua_pushvalue(L, 2);
    lua_setupvalue(L, -2, 1);
    lua_call(L, 0, LUA_MULTRET);

    return 0;
}

static int target(lua_State *L) {
    // Grab the build graph
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_gettable(L, LUA_REGISTRYINDEX);
    BuildGraph *graph = (BuildGraph*) lua_touserdata(L, -1);
    lua_pop(L, 1);

    // Grab the name and the rule
    lua_getfield(L, -1, "name");
    lua_getfield(L, -2, "build");

    Rule *rule = rule_check(L, -1);
    luaL_argcheck(L, rule != NULL, 1, "Unexpected non-rule type received");
    lua_pop(L, 1);

    std::string name = lua_tostring(L, -1);
    lua_pop(L, 1);

    // Create a copy of the rule
    auto rule_copy = std::make_shared<Rule>(*rule);
    assert(rule_copy.get() != rule);

    // Create a new target with the rule copy
    auto target = std::make_shared<Target>(name, rule_copy);
    graph->addTarget(target);

    return 0;
}

static int find_target(lua_State *L) {
    // Grab the build graph
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_gettable(L, LUA_REGISTRYINDEX);
    BuildGraph *graph = (BuildGraph*) lua_touserdata(L, -1);
    lua_pop(L, 1);

    const char* name = lua_tostring(L, 1);
    auto target = graph->findTarget(name);

    if (target == NULL) {
        lua_pushnil(L);
        return 1;
    }

    auto rule = target->getRule();
    auto ins = rule->getInputs();
    auto outs = rule->getOutputs();
    auto cmds = rule->getCommands();

    lua_newtable(L); // top level table

    lua_newtable(L); // outs
    for (size_t i = 0; i < outs.size(); i++) {
        lua_pushstring(L, outs[i].c_str());
        lua_seti(L, -2, i + 1);
    }

    lua_newtable(L); // ins
    for (size_t i = 0; i < ins.size(); i++) {
        lua_pushstring(L, ins[i].c_str());
        lua_seti(L, -2, i + 1);
    }

    lua_newtable(L); // cmds
    for (size_t i = 0; i < cmds.size(); i++) {
        lua_pushstring(L, cmds[i].c_str());
        lua_seti(L, -2, i + 1);
    }

    // reversed order here since we take items
    // off the tstack
    lua_setfield(L, -4, "cmds");
    lua_setfield(L, -3, "ins");
    lua_setfield(L, -2, "outs");

    return 1;
}

int glob(lua_State *L) {
    const char* pattern = lua_tostring(L, 1);
    std::vector<std::string> matches;
    if (fglob(pattern, matches)) {
        std::string err = std::string("Unable to match pathnames from glob: ") + pattern;
        lua_pushstring(L, err.c_str());
        lua_error(L);
        return 0;
    }

    lua_newtable(L);
    for (size_t i = 0; i < matches.size(); i++) {
        lua_pushstring(L, matches[i].c_str());
        lua_seti(L, -2, i + 1);
    }

    return 1;
}

Runtime::Runtime() {
    L = luaL_newstate();
    graph = std::make_unique<BuildGraph>();
}

Runtime::~Runtime() {
    lua_close(L);
}

void Runtime::loadLibs() {
    // Load the standard lua libraries
    luaL_openlibs(L);

    // Setup the rule metatable
    luaL_newmetatable(L, kRuleMetatableMarker);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, rule_gc);
    lua_settable(L, -3);

    // Load the path library
    luaopen_path(L);
}

void Runtime::loadGlobals() {
    // the global submodule function
    lua_pushcfunction(L, submodule);
    lua_setglobal(L, "_bore_submodule");

    // the global target function
    lua_pushcfunction(L, target);
    lua_setglobal(L, "_bore_target");

    // the global rule function
    lua_pushcfunction(L, rule);
    lua_setglobal(L, "_bore_rule");

    // the global find target function
    lua_pushcfunction(L, find_target);
    lua_setglobal(L, "_bore_find_target");

    // the global find target function
    lua_pushcfunction(L, glob);
    lua_setglobal(L, "_bore_glob");

    // push the build graph into the regustry
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_pushlightuserdata(L, (void *) graph.get());
    lua_settable(L, LUA_REGISTRYINDEX);
}

std::unique_ptr<BuildGraph> Runtime::loadAndEvaluate(const std::string &corepath,
                                                     const std::string &modulepath) {

    // loadAndEvalueate can only be called once because it taints
    // the lua runtime
    assert(graph != nullptr);

    loadLibs();
    loadGlobals();

    if (luaL_dofile(L, corepath.c_str())) {
        throw ConfigurationException(lua_tostring(L, -1));
    }

    int t = lua_getglobal(L, "submodule");
    if (t != LUA_TFUNCTION) {
        throw ConfigurationException("Problem loading core module at '" + corepath + "'");
    }

    lua_pushnil(L);
    lua_pushstring(L, modulepath.c_str());
    if (lua_pcall(L, 2, LUA_MULTRET, 0)) {
        throw ConfigurationException(lua_tostring(L, -1));
    }

    return std::move(graph);
}

