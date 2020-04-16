
defnrule("phony", {
    validator = function(args)
        local all_ins = doassert(function()
            assert_table(args.deps, "Expected phony deps to be targets")
            local c = {}
            for _, v in pairs(args.deps) do
                local ins = assert_strings(v.ins, "Expected phony deps to be targets")
                for _, input in pairs(ins) do
                    table.insert(c, input)
                end
            end

            return c
        end)

        return {
            ins = all_ins,
            outs = {},
            cmds = {}
        }
    end,
    generator = function(args)
        return rule(args)
    end
})

