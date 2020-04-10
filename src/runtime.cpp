#include <cassert>
#include <iostream>
#include <filesystem>

#include "runtime.h"
#include "configuration_exception.h"

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

void Runtime::evaluateBuildModule(const std::string& filepath) {

    // Load the file into a chunk  and calling, expecing one return value
    // (the module)
    int err = luaL_loadfile(L, filepath.c_str()) || lua_pcall(L, 0, 1, 0);
    if (err) {
        throw ConfigurationException(lua_tostring(L, -1));
    }

    if (!lua_istable(L, -1)) {
        throw ConfigurationException("Expected build file to return a module", filepath);
    }

    lua_getfield(L, -1, "name");

    size_t len;
    const char* modulename = lua_tolstring(L, -1, &len);

    if (modulename == NULL) {
        throw ConfigurationException("Build module should define a name", filepath);
    }

    if (len == 0) {
        throw ConfigurationException("Build Module name cannot be empty", filepath);
    }

    auto module = std::make_shared<BuildModule>(modulename, filepath);
    graph->addModule(module);

    lua_pop(L, 1);
    lua_getfield(L, -1, "targets");
    if (!lua_istable(L, -1)) {
        throw ConfigurationException("Buld module defines no targets", filepath);
    }

    extractTargets(*module);
    
    // Clear the stack before returning
    lua_settop(L, 0);
}

void Runtime::evaluateBuildScript(const std::string& filepath) {
    int status = luaL_dofile(L, filepath.c_str());
    if (status) {
        throw ConfigurationException(lua_tostring(L, -1));
    }

    // Clear the stack before returning
    lua_settop(L, 0);
}

void Runtime::extractTargets(BuildModule& module) {

    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        if (!lua_istable(L, -1)) {
            throw ConfigurationException("Expected module targets to be a table", module.getBuildFilePath());
        }

        if (!lua_isstring(L, -2)) {
            throw ConfigurationException("Expected module targets to be keyed by string",
                    module.getBuildFilePath());
        }

        auto target = std::make_shared<Target>(lua_tostring(L, -2));
        extractRules(*target, module);
        module.addTarget(target);

        lua_pop(L, 1);
    }
}

void Runtime::extractRules(Target& target, BuildModule& module) {
    int starttop = lua_gettop(L);

    std::cerr << "Target: " << module.getName() << "." << target.getName() << std::endl;

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
                extractRules(target, module);
            }

            lua_pop(L, 1);
        }

    } else {
        if (!lua_istable(L, -1)) {
            std::stringstream ss;
            ss << "In target '" << target.getName() << "': Commands must be an array of strings";
            throw ConfigurationException(ss.str(), module.getBuildFilePath());
        }

        auto rule = std::make_shared<Rule>();

        // Adding the commands
        lua_pushnil(L);  /* first key */
        while (lua_next(L, -2) != 0) {
            if (!lua_isinteger(L, -2) || !lua_isstring(L, -1)) {
                std::stringstream ss;
                ss << "In target '" << target.getName() << "': Commands must be an array of strings";
                throw ConfigurationException(ss.str(), module.getBuildFilePath());
            }

            rule->addCommand(lua_tostring(L, -1));
            std::cerr << "Command: " << lua_tostring(L, -1) << std::endl;
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
                    std::stringstream ss;
                    ss << "In target '" << target.getName()
                        << "': Outputs must be a string or array of strings";
                    throw ConfigurationException(ss.str(), module.getBuildFilePath());
                }

                rule->addOutput(lua_tostring(L, -1));
                std::cerr << "Output: " << lua_tostring(L, -1) << std::endl;
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
                    std::stringstream ss;
                    ss << "In target '" << target.getName()
                        << "': Inputs must be a string or  array of strings";
                    throw ConfigurationException(ss.str(), module.getBuildFilePath());
                }

                rule->addInput(lua_tostring(L, -1));
                std::cerr << "Input: " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1);
            }
        }

        target.addRule(rule);

        // Remember to do this one last time
        lua_pop(L, 1);

    }

    int endtop = lua_gettop(L);
    assert(endtop == starttop);
}

