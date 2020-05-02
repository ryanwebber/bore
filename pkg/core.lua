
local root_build_dir = __bore.build_path
local root_proj_dir = __bore.project_path

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

local function optional_type(var, vtype, default)
    if type(var) == vtype then
        return var
    else
        return default
    end
end

local function optional_string(var, def)      return optional_type(var, "string", def) end
local function optional_table(var, def)       return optional_type(var, "table", def) end
local function optional_number(var, def)      return optional_type(var, "number", def) end
local function optional_function(var, def)    return optional_type(var, "function", def) end

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

local function extract_strings(var)
    if type(var) == "string" then
        return { var }
    elseif type(var) == "table" then
        local strs = {}
        for _, val in pairs(var) do
            local extracted = extract_strings(val)
            for _, s in pairs(extracted) do
                table.insert(strs, s)
            end
        end

        return strs
    else
        return {}
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

local array = {
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
    map = function(arr, fn)
        local t = {}
        local i = 1
        for k, v in pairs(arr) do
            local k0, v0 = fn(k, v, i)
            t[k0] = v0
            i = i + 1
        end

        return t
    end,
    join = function(arr)
        return varsub_tostr(arr)
    end
}

local include = function (relative_path)
    doassert(function()
        assert_string(relative_path, "Build template path should be a string")
    end)

    local build_file = path.join(root_proj_dir, relative_path)
    local qualified_dir = path.dirname(build_file);
    local local_dir = path.dirname(relative_path)

    local template_env = {
        build_dir = root_build_dir,
        local_dir = local_dir,
        path = function(...)
            return path.join(local_dir, ...)
        end,
        object = function(...)
            return path.join(root_build_dir, ...)
        end,
        glob = function(...)
            local results = glob(path.join(qualified_dir, ...))
            local retval = {}
            for _, part in ipairs(results) do
                if part:sub(1, #qualified_dir) == qualified_dir then
                    table.insert(retval, part:sub(#qualified_dir + 1))
                else
                    table.insert(retval, part)
                end
            end

            return retval
        end,
    }

    local env = setmetatable({ env = template_env }, {
        __index = function(self, k)
            if k == "__bore" then
                -- Hide the internal APIs
                return nil
            else
                return _G[k]
            end
        end
    })

    __bore.include(build_file, env)
end

local targets = setmetatable({}, {
    __index = function(_, name)
        doassert(function()
            assert_string(name, "Target index must be a string")
        end)

        return __bore.find_target(name)
    end
})

local target = function (args)
    doassert(function()
        assert_table(args, "Target args must be a table")
        assert_string(args.name, "Target name must be a string")
        assert_rule(args.build, "Target build property must be a rule")

        if targets[args.name] ~= nil then
            fatal("Target '%s' already defined", args.name)
        end
    end)

    __bore.target({
        name = args.name,
        description = args.description,
        build = args.build,
        phony = args.phony == true,
        default = args.default == true,
        alias = args.alias ~= false,
    })

    doassert(function()
        if args.phony == true and #targets[args.name].outs > 0 then
            fatal("Targets marked as phony should provide no outputs")
        end
    end)

    return targets[args.name]
end

-- Standard rules
local rules = {
    rule = function(args)

        doassert(function()
            assert_table(args, "Rule expected table")
        end)

        local ins = extract_strings(args.ins)
        local outs = extract_strings(args.outs)
        local cmds = extract_strings(args.cmds)
        local dirs = args.dirs

        if type(dirs) == "string" then
            dirs = { dirs }
        elseif type(dirs) ~= "table" then
            dirs = {}
            for _, out in pairs(outs) do
                local dir = path.dirname(out)
                if type(dir) == "string" and #dir > 0 then
                    table.insert(dirs, dir)
                end
            end
        end

        local r = {
            ins = ins,
            outs = outs,
            cmds = cmds,
            dirs = dirs
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

        -- We varsub only the commands array
        r.cmds = array.map(r.cmds, function(k, v) return k, varsub(v, data) end)

        return __bore.rule(r)
    end
}

local config = setmetatable(__bore.config, {})

local assert = {
    string = assert_string,
    number = assert_number,
    func = assert_function,
    table = assert_table,
    bubble = doassert,
}

local optional = {
    string = optional_string,
    number = optional_number,
    func = optional_func,
    table = optional_table,
    strings = function(value, default)
        if type(value) == "string" then
            return { value }
        elseif type(value) == "table" then
            return extract_strings(value)
        else
            return default
        end
    end,
}

local globals = {
    include = include,
    target = target,
    targets = targets,
    config = config,
    rules = rules,
}

-- Update the global metatable to resolve the defined rules,
-- and catch undefined global variable access
setmetatable(_G, {
    __index = function(self, k)
        local value = rawget(_G, k)
        if value ~= nil then
            return value
        elseif globals[k] ~= nil then
            return globals[k]
        elseif rules[k] ~= nil then
            return rules[k]
        else
            error(string.format("access of undefined variable '%s'", k), 2)
        end
    end
})

