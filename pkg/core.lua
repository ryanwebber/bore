
-- Fails without a description of where the error occurs
-- to avoid leaking internal details
local fatal = function(...)
    local msg = string.format(...)
    error(msg, 0)
end

-- Catches any errors thrown anywere in the call hierarchy and re-throws them at the location
-- of the callers caller. (ex. if A calls B(), and B calls assert(), then it throws
-- the error in A's callsite of B
local doassert = function(fn)
    local success, results  = pcall(fn)
    if not success then
        error(results, 3)
    end

    return results
end

local assert_type = function(var, vtype, msg)
    if type(var) ~= vtype then
        fatal("%s (got: %s)", msg, type(var))
    end

    return var
end

local assert_string =    function(var, msg) return assert_type(var, "string", msg)   end
local assert_table =     function(var, msg) return assert_type(var, "table", msg)    end
local assert_number =    function(var, msg) return assert_type(var, "number", msg)   end
local assert_function =  function(var, msg) return assert_type(var, "function", msg) end

local assert_rule = function(var, msg)
    assert_type(var, "userdata", msg)
end

local assert_strings = function(var, msg)
    if type(var) == "string" then
        return { var }
    elseif type(var) == "table" then
        return var
    else
        fatal("%s (got: %s)", msg, type(var))
    end
end

local validate = function(var, spec)
    local t = {}
    for pname, pspec in pairs(spec) do
        if var[pname] == nil and pspec.required == true then
            fatal("Validation error: field '%s' cannot be nil", pname)
        elseif var[pname] == nil and pspec.default ~= nil then
            t[pname] = pspec.default
        elseif type(pspec.type) == "string" and type(var[pname]) ~= pspec.type then
            fatal("Validation error: field '%s' expected to be of type '%s' but was '%s'",
                    pname, pspec.type, type(var[pname]))
        else
            t[pname] = var[pname]
        end
    end

    return t
end

-- A global list accumulating custom rule definitions
local ruledefs = {}

-- The core lua component that makes build modules spit out the build tree
defnrule = function(name, def)

    doassert(function()
        assert_string(name, "Rule name must be a string")
        assert_table(def, "Rule definition must be a table")
        assert_function(def.generator, "Rule definition must have a generator field")

        if ruledefs[name] ~= nil then
            fatal("Rule '%s' is already defined", name)
        end
    end)

    ruledefs[name] = function(args)
        local validated_args = doassert(function()
            assert_table(args, string.format("Arguments to rule '%s' must be a table", name))
            if type(def.validator) == "table" then
                return validate(args, def.validator)
            elseif type(def.validator) == "function" then
                return def.validator(args)
            else
                return args
            end
        end)

        return def.generator(validated_args)
    end

    return ruledefs[name]

end

glob = function(pattern)
    doassert(function()
        assert_string(pattern, "Glob pattern should be a string")
    end)

    return _bore_glob(pattern)
end

submodule = function (mod, relpath)
    doassert(function()
        assert_string(relpath, "Submodule path must be a string")
    end)

    local buildfile = relpath
    if mod ~= nil then
        buildfile = path.join(mod.local_dir, relpath)
    end

    local local_dir = path.dirname(buildfile)

    local module = {
        root_dir = "",
        root_build_dir = "",
        local_dir = local_dir,
        local_build_dir = "",
        rel = function(p)
            doassert(function()
                assert_string(p, "Path must be a string")
            end)

            return path.join(local_dir, p)
        end,
        abs = function(p)
            doassert(function()
                assert_string(p, "Path must be a string")
            end)

            return path.join(root_dir, p)
        end,
    }

    local env = setmetatable({ module = module }, { __index = _G })
    _bore_submodule(buildfile, env)
end

targets = setmetatable({}, {
    __index = function(_, name)
        doassert(function()
            assert_string(name, "Target index must be a string")
        end)

        return _bore_find_target(name)
    end
})

target = function (args)
    doassert(function()
        assert_table(args, "Target args must be a table")
        assert_string(args.name, "Target name must be a string")
        assert_rule(args.build, "Target build property must be a rule")

        if targets[args.name] ~= nil then
            fatal("Target '%s' already defined", args.name)
        end
    end)

    _bore_target(args)
end

defnrule("rule", {
    validator = function(args)
        return {
            ins = assert_strings(args.ins, "Rule inputs should be a string array"),
            outs = assert_strings(args.outs, "Rule outputs should be a string array"),
            cmds = assert_strings(args.cmds, "Rule commands should be a string array"),
        }
    end,
    generator = function(args)
        return _bore_rule(args)
    end
})

-- TODO: phony rule isn't really working
defnrule("phony", {
    generator = function(args)
        return rule {
            ins = {},
            outs = {},
            cmds = {}
        }
    end
})

-- Setup global utilities
bore = {
    -- Assertions
    assert_string = assert_string,
    assert_strings = assert_strings,
    assert_number = assert_number,
    assert_function = assert_function,
    assert_table = assert_table,
    validate = validate,
}

-- Update the global metatable to resolve the defined rules,
-- and catch undefined global variable access
setmetatable(_G, {
    __index = function(self, k)
        local value = rawget(_G, k)
        if value ~= nil then
            return value
        elseif type(ruledefs[k]) == "function" then
            return ruledefs[k]
        else
            error(string.format("access of undefined variable '%s'", k), 2)
        end
    end
})

