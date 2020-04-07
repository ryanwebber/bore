
-- Define our own copy rule
defnrule("copy", {
    generator = function (args)
        return rule {
            ins = args.from,
            outs = args.to,
            cmds = {
                "cp ${ins} ${outs}"
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

return M

