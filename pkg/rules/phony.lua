defnrule("phony", {
    validator = function(args)
        local outs = {}
        local cmds = {}

        if type(args.deps) == "table" then
            outs = assert.bubble(function()
                local c = {}
                for _, v in pairs(args.deps) do
                    local outs = assert.strings(v.outs, "Expected phony deps to be targets")
                    for _, output in pairs(outs) do
                        table.insert(c, output)
                    end
                end

                return c
            end)
        end

        if type(args.cmds) == "table" then
            cmds = assert.bubble(function()
                return assert.strings(args.cmds, "Expected phony cmds to be strings")
            end)
        elseif type(args.cmds) == "string" then
            cmds = { args.cmds }
        end

        return {
            ins = outs,
            outs = {},
            cmds = cmds
        }
    end,
    generator = function(args)
        return rule(args)
    end
})

