#include <cassert>
#include <iostream>
#include <filesystem>

#include "runtime.h"
#include "configuration_exception.h"

static const char *kRuleMetatableMarker = "Bore.RuleMarker";
static const char *kBuildGraphRegistryMarker = "__usedAsAddress";

namespace fs = std::filesystem;

static int rule(lua_State *L) {
    std::cerr << "Rule created" << std::endl;

    // We're going to return a userdata containing a Rule*.
    // We'll also define a metatable for it to check that
    // it's actually a rule, and we can create a gc method
    // for it that will call delete on the constining Rule.
    //
    // We'll also need to add a copy constructor onto Rule
    // that we'll call when we insert the rule into the build
    // graph through target() so that when Rule is gc'd we don't
    // keep invalid references to it

    Rule **prule = reinterpret_cast<Rule**>(lua_newuserdata(L, sizeof(Rule*)));
    *prule = new Rule();

    // TODO: Add stuff to the rule
    Rule &rule = **prule;
    rule.addOutput("hello.txt");

    // TODO: Assign the right metatable to the rule
    luaL_getmetatable(L, kRuleMetatableMarker);
    lua_setmetatable(L, -2);

    return 1;
}

static Rule* rule_check(lua_State *L, int index)
{
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
    std::cerr << "Submodule called" << std::endl;
    return 0;
}

static int target(lua_State *L) {
    std::cerr << "Target created" << std::endl;

    lua_getfield(L, -1, "name");
    lua_getfield(L, -2, "build");

    Rule *rule = rule_check(L, -1);
    luaL_argcheck(L, rule != NULL, 1, "Unexpected non-rule type received");

    for (auto output : rule->getOutputs()) {
        std::cerr << "Target has output: " << output << std::endl;
    }

    /*
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_gettable(L, LUA_REGISTRYINDEX);
    BuildGraph *graph = (BuildGraph*) lua_touserdata(L, -1);
    */

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
    // Load the standard lua libraries
    luaL_openlibs(L);

    // Setup the rule metatable
    luaL_newmetatable(L, kRuleMetatableMarker);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, rule_gc);
    lua_settable(L, -3);
}

void Runtime::loadGlobals() {
    // the global submodule function
    lua_pushcfunction(L, submodule);
    lua_setglobal(L, "_submodule");

    // the global target function
    lua_pushcfunction(L, target);
    lua_setglobal(L, "_target");

    // the global rule function
    lua_pushcfunction(L, rule);
    lua_setglobal(L, "_rule");

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

