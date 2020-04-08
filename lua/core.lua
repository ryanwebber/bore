
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
        fatal("%s", msg)
    end
end

local assert_string =    function(var, msg) assert_type(var, "string", msg)   end
local assert_table =     function(var, msg) assert_type(var, "table", msg)    end
local assert_number =    function(var, msg) assert_type(var, "number", msg)   end
local assert_function =  function(var, msg) assert_type(var, "function", msg) end

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

-- The most primitive rule definition just returns the inputs and outputs
-- it's given
defnrule("rule", {
    generator = function(args)
        return args
    end
})

-- Setup global utilities
bore = {
    -- Assertions
    assert_string = assert_string,
    assert_number = assert_number,
    assert_function = assert_function,
    assert_table = assert_table,
    validate = validate,

    -- Rule utilities
    defnrule = defnrule,
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

