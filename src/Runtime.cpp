#include <cassert>
#include <iostream>
#include <filesystem>
#include "runtime.h"

namespace fs = std::filesystem;

Runtime::Runtime() {
    L = luaL_newstate();
    graph = std::make_shared<BuildGraph>();
}

Runtime::~Runtime() {
    lua_close(L);
}

std::shared_ptr<BuildGraph> Runtime::getBuildGraph() const {
    return graph;
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
        std::cerr << lua_tostring(L, -1) << std::endl;
        return false;
    }

    if (!lua_istable(L, -1)) {
        std::cerr << "Expected build file to return a module: " << filepath << std::endl;
        return false;
    }

    lua_getfield(L, -1, "name");

    size_t len;
    const char* modulename = lua_tolstring(L, -1, &len);

    if (modulename == NULL) {
        std::cerr << "Module must define a name: " << filepath << std::endl;
        return false;
    }

    if (len == 0) {
        std::cerr << "Module name cannot be empty: " << filepath << std::endl;
        return false;
    }

    auto module = std::make_shared<BuildModule>(modulename, filepath);
    if (!graph->insertModule(module)) {
        std::cerr << "Build module with name '" << modulename << "' already exists: "
            << filepath << std::endl;
        return false;
    }

    lua_pop(L, 1);
    lua_getfield(L, -1, "targets");
    if (!lua_istable(L, -1)) {
        std::cerr << "Warning: Build module defines no targets: " << filepath << std::endl;
        return false;
    }

    // The table is at the top of the stack, recursively extract
    // the returned targets
    extractTargets(*module);

    // Clear the stack before returning
    lua_settop(L, 0);

    return !err;
}

bool Runtime::evaluateBuildScript(const std::string& filepath) {
    int status = luaL_dofile(L, filepath.c_str());
    if (status) {
        std::cerr << lua_tostring(L, -1) << std::endl;
        return false;
    }

    // Clear the stack before returning
    lua_settop(L, 0);

    return true;
}

bool Runtime::extractTargets(BuildModule& module) {

    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if (!lua_istable(L, -1)) {
            std::cerr << "Module targets should be a collection of rules (got '" << luaL_typename(L, -1)
                << "'): " << module.getBuildFilePath() << std::endl;

            return false;
        }

        if (!lua_isstring(L, -2)) {
            std::cerr << "Module targets key should be a string (got '" << luaL_typename(L, -2)
                << "'): " << module.getBuildFilePath() << std::endl;

            return false;
        }

        const char* target_name = lua_tostring(L, -2);
        std::cerr << "Discovered target: " << target_name << std::endl;

        auto target = std::make_shared<Target>(target_name);
        if (!extractRules(*target)) {
            return false;
        }

        module.addTarget(target);

        lua_pop(L, 1);
    }

    return true;
}

bool Runtime::extractRules(Target& target) {
    int starttop = lua_gettop(L);

    // TODO: Replace 'rule' rule with userdata, and check metatable
    // here
    lua_getfield(L, -1, "ins");
    lua_getfield(L, -2, "outs");
    lua_getfield(L, -3, "cmds");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 3);
        lua_pushnil(L);
        while(lua_next(L, -2) != 0) {
            if (lua_istable(L, -1)) {
                if (!extractRules(target)) {
                    return false;
                }
            }

            lua_pop(L, 1);
        }

    } else {
        if (!lua_istable(L, -1) || lua_rawlen(L, -1) == 0) {
            std::cerr << "In target '" << target.getName() << "': Rule does not contain any commands"
                << std::endl;

            return false;
        }

        auto rule = std::make_shared<Rule>();

        // Adding the commands
        lua_pushnil(L);  /* first key */
        while (lua_next(L, -2) != 0) {
            if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                std::cerr << "In target '" << target.getName()
                    << "': Expected commands to be an array of strings" << std::endl;

                return false;
            }

            std::cerr << "Command: " << lua_tostring(L, -1) << std::endl;
            rule->addCommand(lua_tostring(L, -1));

            lua_pop(L, 1);
        }

        // Pop back to outputs
        lua_pop(L, 1);

        // Adding the outputs
        if (lua_isstring(L, -1)) {
            std::cerr << "Output: " << lua_tostring(L, -1) << std::endl;
            rule->addOutput(lua_tostring(L, -1));
        } else if (lua_istable(L, -1) && lua_rawlen(L, -1) > 0) {
            lua_pushnil(L);  /* first key */
            while (lua_next(L, -2) != 0) {
                if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                    std::cerr << "In target '" << target.getName()
                        << "': Expected outputs to be an array of strings" << std::endl;

                    return false;
                }

                std::cerr << "Output: " << lua_tostring(L, -1) << std::endl;
                rule->addOutput(lua_tostring(L, -1));

                lua_pop(L, 1);
            }
        }

        // Pop back to inputs
        lua_pop(L, 1);

        // Adding the inputs
        if (lua_isstring(L, -1)) {
            std::cerr << "Input: " << lua_tostring(L, -1) << std::endl;
            rule->addOutput(lua_tostring(L, -1));
        } else if (lua_istable(L, -1) && lua_rawlen(L, -1) > 0) {
            lua_pushnil(L);  /* first key */
            while (lua_next(L, -2) != 0) {
                if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                    std::cerr << "In target '" << target.getName()
                        << "': Expected inputs to be an array of strings" << std::endl;

                    return false;
                }

                std::cerr << "Input: " << lua_tostring(L, -1) << std::endl;
                rule->addInput(lua_tostring(L, -1));

                lua_pop(L, 1);
            }
        }

        target.addRule(rule);

        // Remember to do this one last time
        lua_pop(L, 1);

    }

    int endtop = lua_gettop(L);
    assert(endtop == starttop);

    return true;
}

