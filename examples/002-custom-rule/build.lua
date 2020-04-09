
-- Define our own copy rule
defnrule("copy", {
    validator = function(args)
        if type(args.from) == "string" then
            args.from = { args.from }
        end

        return bore.validate(args, {
            from = {
                type = "table",
                required = true,
            },
            to = {
                type = "string",
                required = true
            }
        })
    end,
    generator = function (args)
        return rule {
            ins = args.from,
            outs = args.to,
            cmds = {
                string.format("cp %s %s", table.concat(args.from, " "), args.to),
                "rm -rf /tmp/bore-garbage",
                "echo yay",
            }
        }
    end
})

local M = {
    name = "main",
    targets = {}
}

M.targets.main = copy {
    from = "input.txt",
    to = "output.txt"
}

M.targets.main2 = copy {
    from = { "output.txt", "other.txt" },
    to = "final.txt"
}

return M

