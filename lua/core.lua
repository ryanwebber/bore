
local ruledefs = {}

local assertType = function(var, vtype, msg)
    if type(var) ~= vtype then
        error(msg)
    end
end

local assertString = function(var, msg) assertType(var, "string", msg) end
local assertTable = function(var, msg) assertType(var, "table", msg) end
local assertNumber = function(var, msg) assertType(var, "number", msg) end

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

defnrule("rule", {
    generator = function(args)
        return args
    end
})

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

