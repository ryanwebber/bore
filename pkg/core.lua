
local root_build_dir = _bore_build_path

-- Fails without a description of where the error occurs
-- to avoid leaking internal details
local function fatal(...)
    local msg = string.format(...)
    error(msg, 0)
end

-- Catches any errors thrown anywere in the call hierarchy and re-throws them at the location
-- of the callers caller. (ex. if A calls B(), and B calls assert(), then it throws
-- the error in A's callsite of B
local function doassert(fn)
    local success, results  = pcall(fn)
    if not success then
        error(results, 3)
    end

    return results
end

local function assert_type(var, vtype, msg)
    if type(var) ~= vtype then
        fatal("%s (got: %s)", msg, type(var))
    end

    return var
end

local function assert_string   (var, msg) return assert_type(var, "string", msg)   end
local function assert_table    (var, msg) return assert_type(var, "table", msg)    end
local function assert_number   (var, msg) return assert_type(var, "number", msg)   end
local function assert_function (var, msg) return assert_type(var, "function", msg) end

local function assert_rule(var, msg)
    assert_type(var, "userdata", msg)
end

local function assert_strings(var, msg)
    if type(var) == "string" then
        return { var }
    elseif type(var) == "table" then
        local strs = {}
        for _, val in pairs(var) do
            local extracted = assert_strings(val, msg)
            for _, s in pairs(extracted) do
                table.insert(strs, s)
            end
        end

        return strs
    else
        fatal("%s (got: %s)", msg, type(var))
    end
end

local function validate(var, spec)
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

local function varsub_tostr(obj)
    if type(obj) == "string" then
        return obj
    elseif type(obj) == "number" then
        return tostring(obj)
    elseif type(obj) == "table" then
        local str = ""
        for _, p in pairs(obj) do
            local possible = varsub_tostr(p)
            if possible:len() > 0 then
                if str:len() > 0 then
                    str = str .. " "
                end

                str = str .. possible
            end
        end
        return str
    else
        return ""
    end
end

local function varsub(str, data)
    local i = str:gsub("${(%a[%w%.]*)}", function (key)
        local value = data
        for prop in key:gmatch("(%a%w*)") do
            if prop:len() > 0 then
                local v = value[prop]
                if v == nil then
                    return nil
                end

                value = v
            end
        end

        return varsub_tostr(value)
    end)

    return varsub_tostr(i)
end

local function map(arr, fn)
    local t = {}
    for k, v in pairs(arr) do
        t[k] = fn(k, v)
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
    local local_build_dir = path.join(root_build_dir, local_dir)

    local module = {
        root_dir = "",
        root_build_dir = root_build_dir,
        local_dir = local_dir,
        local_build_dir = local_build_dir,
        path = function(p)
            doassert(function()
                assert_string(p, "Path must be a string")
            end)

            return path.join(local_dir, p)
        end,
        object = function(p)
             doassert(function()
                assert_string(p, "Path must be a string")
            end)

            return path.join(local_build_dir, p)
        end,
        glob = function(p)
            doassert(function()
                assert_string(p, "Glob pattern must be a string")
            end)

            return glob(path.join(local_dir, p))
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
    return targets[args.name]
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

        local r = {
            cmds = args.cmds,
            ins = args.ins,
            outs = args.outs
        }

        local data = setmetatable(r, {
            __index = function(self, k)
                local raw = rawget(self, k)
                if raw ~= nil then
                    return raw
                end

                return targets[k]
            end
        })

        r.ins = map(r.ins, function(_, v) return varsub(v, data) end)
        r.outs = map(r.outs, function(_, v) return varsub(v, data) end)

        -- important that cmds comes last here, since we've alreadt expanded
        -- the inputs and outputs
        r.cmds = map(r.cmds, function(_, v) return varsub(v, data) end)

        return _bore_rule(r)
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

array = {
    concat = function(...)
        local t = {}
        for _, ti in pairs(...) do
            doassert(function () assert_table(ti, "Arg given to array.concat was not a table") end)
            for _, el in pairs(ti) do
                table.insert(t, el)
            end
        end

        return t
    end,
    map = map
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

