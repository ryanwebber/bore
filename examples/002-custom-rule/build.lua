
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
                string.format("cp %s %s", table.concat(args.from, " "), args.to)
            }
        }
    end
})

local M = {
    name = "",
    targets = {}
}

M.targets.main = copy {
    from = "input.txt",
    to = "output.txt"
}

-- REMOVE ME
print(M.targets.main.cmds[1])

return M

