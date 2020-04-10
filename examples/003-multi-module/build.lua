
local M = {
    name = "main",
    targets = {}
}

M.targets.all = rule {
    ins = build.modules.greeting.main.outs,
    outs = {},
    cmds = {}
}

return M

