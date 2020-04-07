
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

-- A global list accumulating custom rule definitions
local ruledefs = {}

local assertType = function(var, vtype, msg)
    if type(var) ~= vtype then
        error(msg)
    end
end

local assertString = function(var, msg) assertType(var, "string", msg) end
local assertTable = function(var, msg) assertType(var, "table", msg) end
local assertNumber = function(var, msg) assertType(var, "number", msg) end

-- The core lua component that makes build modules spit out the build tree
defnrule = function(name, def)

    assertString(name, "Rule name must be a string")
    assertTable(def, "Rule definition must be a table")

    if ruledefs[name] ~= nil then
        error(string.format("Rule '%s' is already defined", name))
    end

    ruledefs[name] = function(args)
        local parts = def.generator(args)
        return parts
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
            error(string.format("access of undefined variable '%s'", k))
        end
    end
})

