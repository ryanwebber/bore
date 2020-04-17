defnrule("phony", {
    validator = function(args)
        local all_outs = doassert(function()
            assert_table(args.deps, "Expected phony deps to be targets")
            local c = {}
            for _, v in pairs(args.deps) do
                local outs = assert_strings(v.outs, "Expected phony deps to be targets")
                for _, output in pairs(outs) do
                    table.insert(c, output)
                end
            end

            return c
        end)

        return {
            ins = all_outs,
            outs = {},
            cmds = {}
        }
    end,
    generator = function(args)
        return rule(args)
    end
})

