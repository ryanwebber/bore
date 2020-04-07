
-- Define our own copy rule
defnrule("copy", {
    args = {
        from = {
            type = { "string", "table" },
            required = true,
        },
        to = {
            type = "string",
            required = true
        }
    },
    generator = function (args)

        local outs = args.to
        local ins = args.from
        if type(ins) == "string" then
            ins = { ins }
        end

        return rule {
            ins = ins,
            outs = outs,
            cmds = {
                string.format("cp %s %s", table.concat(ins, " "), outs)
            }
        }
    end
})

local M = {
    targets = {}
}

M.targets.main = copy {
    from = "input.txt",
    to = "output.txt"
}

-- REMOVE ME
print(M.targets.main.cmds[1])

return M

