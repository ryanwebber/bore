
-- TODO: Remove me when we can land bore utils native
bore = {
    path = {
    }
}

-- TODO: Remove me when build context are provided this global
build = {
    root_dir = "",
    root_build_dir = "",
    local_dir = "",
    local_build_dir = "",
    get_module = function(path)
        return {
            inputs = { "pretend_input.txt" },
            outputs = { "pretend_output.txt" }
        }
    end,
    get_option = function(key)
        return "value-for-" .. key
    end
}

-- Fails without a description of where the error occurs
-- to avoid leaking internal details
local fatal = function(...)
    local msg = string.format(...)
    error(msg, 0)
end

-- Catches any errors thrown anywere in the call hierarchy and re-throws them at the location
-- of the callers caller. (ex. if this is used in a function a user calls, then it throws
-- the error back at their call site)
local assert = function(fn)
    local _, err = pcall(fn)
    if err then
        error(err, 3)
    end
end

local assert_type = function(var, vtype, msg)
    if type(var) ~= vtype then
        fatal("%s", msg)
    end
end

local assert_string =    function(var, msg) assert_type(var, "string", msg)   end
local assert_table =     function(var, msg) assert_type(var, "table", msg)    end
local assert_number =    function(var, msg) assert_type(var, "number", msg)   end
local assert_function =  function(var, msg) assert_type(var, "function", msg) end

local validate = function(var, spec)
    for pname, pspec in pairs(spec) do
        if pspec.required == true and var[pname] == nil then
            fatal("Table field '%s' cannot be nil", pname, tostring(var))
        end
    end
end


-- A global list accumulating custom rule definitions
local ruledefs = {}

-- The core lua component that makes build modules spit out the build tree
defnrule = function(name, def)

    assert(function()
        assert_string(name, "Rule name must be a string")
        assert_table(def, "Rule definition must be a table")
        assert_function(def.generator, "Rule definition must have a generator field")

        if ruledefs[name] ~= nil then
            fatal("Rule '%s' is already defined", name)
        end
    end)

    ruledefs[name] = function(args)
        assert(function()
            assert_table(args, string.format("Arguments to rule '%s' must be a table", name))
            if type(def.args) == "table" then validate(args, def.args) end
        end)

        return def.generator(args)
    end

    return ruledefs[name]

end

-- The most primitive rule definition just returns the inputs and outputs
-- it's given
defnrule("rule", {
    generator = function(args)
        return args
    end
})

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

