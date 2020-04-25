#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "lua_fglob.h"
#include "lua_path.h"
#include "lua_runtime.h"
#include "utils.h"

static const char *kRuleMetatableMarker = "Bore.RuleMarker";
static const char *kBuildGraphRegistryMarker = "__usedAsAddress";

// Generated by the linker that included the binary lua blob
extern const char _binary_build_bundle_lua_start[];
extern const char _binary_build_bundle_lua_end[];

struct RuntimeGlobals {
    const char *build_dir;
    const char *root_dir;
    struct KeyValueList *config;
};

static int rule(lua_State *L) {
    struct Rule **prule = (struct Rule**) lua_newuserdata(L, sizeof(struct Rule*));
    *prule = malloc(sizeof(struct Rule));

    luaL_getmetatable(L, kRuleMetatableMarker);
    lua_setmetatable(L, -2);

    // Parse out the rules, assuming the lua core has already
    // done some basic data formatting and validation for us
    struct Rule *rule = *prule;
    rule_init(rule);

    int s_top_start = lua_gettop(L);

    lua_getfield(L, 1, "deps");
    lua_getfield(L, 1, "dirs");
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
        list_add(&rule->commands, cmd);
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
        list_add(&rule->outputs, output);
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
        list_add(&rule->inputs, input);
        lua_pop(L, 1);
    }

    // Pop back to dirs
    lua_pop(L, 1);

    // Adding the dirs
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            luaL_argerror(L, 1, "Expected dirs to be a list of strings");
        }

        const char* dir = lua_tostring(L, -1);
        list_add(&rule->dirs, dir);
        lua_pop(L, 1);
    }

    // Pop back to deps
    lua_pop(L, 1);

    // Adding the deps
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
            luaL_argerror(L, 1, "Expected deps to be a list of strings");
        }

        const char* dep = lua_tostring(L, -1);
        list_add(&rule->deps, dep);
        lua_pop(L, 1);
    }

    // Pop one last time
    lua_pop(L, 1);

    // General sanity check. The stack gets emptied when we
    // return here anyways
    int s_top_end = lua_gettop(L);
    assert(s_top_start == s_top_end);

    // The rule userdatum is now on the top of the stack
    return 1;
}

static struct Rule* rule_check(lua_State *L, int index) {
    struct Rule *rule;
    luaL_checktype(L, index, LUA_TUSERDATA);

    void* udata = luaL_testudata(L, index, kRuleMetatableMarker);
    if (udata == NULL)
        return NULL;

    rule = *((struct Rule**) udata);
    return rule;
}

static int rule_gc(lua_State *L) {
    struct Rule *rule = rule_check(L, -1);
    luaL_argcheck(L, rule != NULL, 1, "Unexpected non-rule type received");

    rule_free(rule);
    free(rule);

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
    struct BuildGraph *graph = (struct BuildGraph*) lua_touserdata(L, -1);
    lua_pop(L, 1);

    // Grab the name and the rule
    lua_getfield(L, -1, "name");
    lua_getfield(L, -2, "phony");
    lua_getfield(L, -3, "build");

    struct Rule *rule = rule_check(L, -1);
    luaL_argcheck(L, rule != NULL, 1, "Unexpected non-rule type received");
    lua_pop(L, 1);

    bool phony = lua_toboolean(L, -1);
    lua_pop(L, 1);

    const char *name = lua_tostring(L, -1);
    lua_pop(L, 1);

    struct Rule *cpy = malloc(sizeof(struct Rule));
    rule_init(cpy);
    rule_copy(cpy, rule);

    struct Target *target = malloc(sizeof(struct Target));
    target->name = strclone(name);
    target->rule = cpy;
    target->phony = phony;

    struct Error *err = NULL;
    graph_insert_target(graph, target, &err);

    if (err != NULL) {
        lua_pushstring(L, err->msg);
        lua_error(L);
    }

    return 0;
}

static int find_target(lua_State *L) {
    // Grab the build graph
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_gettable(L, LUA_REGISTRYINDEX);
    struct BuildGraph *graph = (struct BuildGraph*) lua_touserdata(L, -1);
    lua_pop(L, 1);

    const char* name = lua_tostring(L, 1);
    struct Target *target = graph_get_target(graph, name);
    if (target == NULL) {
        lua_pushnil(L);
        return 1;
    }

    struct List *elems[] = {
        &target->rule->outputs,
        &target->rule->inputs,
        &target->rule->commands,
        &target->rule->dirs,
        NULL
    };

    const char* fields[] = {
        "outs",
        "ins",
        "cmds",
        "dirs"
    };

    lua_newtable(L); // top level table containing the ins, outs, dirs, and cmds

    int j = 0;
    while (elems[j] != NULL) {
        lua_newtable(L); // the lua array for the list

        int i = 0;
        struct ListNode *n = list_first(elems[j]);
        while (n != NULL) {
            lua_pushstring(L, n->value);
            lua_seti(L, -2, ++i);
            n = list_next(n);
        }

        lua_setfield(L, -2, fields[j]);

        j++;
    }

    lua_pushstring(L, target->name);
    lua_setfield(L, -2, "name");

    return 1;
}

static void runtime_load_libs(struct LuaRuntime *runtime) {
    lua_State *L = runtime->L;

    // Load the standard lua libraries
    luaL_openlibs(L);

    // Setup the rule metatable
    luaL_newmetatable(L, kRuleMetatableMarker);
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, rule_gc);
    lua_settable(L, -3);

    // Load our c 'libraries'
    luaopen_path(L);
    luaopen_fglob(L);
}

static void runtime_load_globals(lua_State *L, struct RuntimeGlobals *globals) {
    lua_pushcfunction(L, submodule);
    lua_setfield(L, -2, "submodule");

    lua_pushcfunction(L, target);
    lua_setfield(L, -2, "target");

    lua_pushcfunction(L, rule);
    lua_setfield(L, -2, "rule");

    lua_pushcfunction(L, find_target);
    lua_setfield(L, -2, "find_target");

    lua_pushstring(L, globals->build_dir);
    lua_setfield(L, -2, "build_path");

    lua_pushstring(L, globals->root_dir);
    lua_setfield(L, -2, "project_path");

    lua_newtable(L);
    struct KeyValueNode *kvn = kvp_first(globals->config);
    while (kvn != NULL) {
        lua_pushstring(L, kvn->value);
        lua_setfield(L, -2, kvn->key);
        kvn = kvp_next(kvn);
    }

    lua_setfield(L, -2, "config");
}

void runtime_init(struct LuaRuntime *runtime) {
    runtime->L = luaL_newstate();
    runtime_load_libs(runtime);
}

void runtime_free(struct LuaRuntime *runtime) {
    lua_close(runtime->L);
}

void runtime_evaluate(struct LuaRuntime *runtime,
        const char* root_dir,
        const char* build_dir,
        const char* build_file,
        struct KeyValueList *config,
        struct BuildGraph *graph,
        struct Error **err) {

    lua_State *L = runtime->L;

    // push the build graph into the regustry
    lua_pushlightuserdata(L, (void *) &kBuildGraphRegistryMarker);
    lua_pushlightuserdata(L, (void *) graph);
    lua_settable(L, LUA_REGISTRYINDEX);

    // Load the embedded bore lua core
    size_t len = _binary_build_bundle_lua_end - _binary_build_bundle_lua_start;
    char data[len + 1];

    // Copy it and null terminate it
    strncpy(data, _binary_build_bundle_lua_start, len);
    data[len] = '\0';

    if(luaL_loadstring (L, data)) {
        return error_fmt(err, "%s", lua_tostring(L, -1));
    }

    struct RuntimeGlobals globals = {
        .build_dir = build_dir,
        .root_dir = root_dir,
        .config = config,
    };

    // Load a table that will hold our __bore.* globals
    // in lua land
    lua_newtable(L);
    int stacktop_a = lua_gettop(L);
    runtime_load_globals(L, &globals);
    int stacktop_b = lua_gettop(L);
    assert(stacktop_a == stacktop_b);

    // Assign the __bore globals
    lua_setglobal(L, "__bore");

    // Evaluate the core
    lua_call(L, 0, LUA_MULTRET);

    // Finally, load the main build module as a submodule and call it
    lua_getglobal(L, "submodule");
    lua_pushnil(L);
    lua_pushstring(L, build_file);

    if (lua_pcall(L, 2, LUA_MULTRET, 0)) {
        return error_fmt(err, "%s", lua_tostring(L, -1));
    }
}

