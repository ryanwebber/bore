
local M = {
    name = "main",
    targets = {}
}

M.targets.all = rule {
    ins = { "transcript.txt" },
    outs = {},
    cmds = {}
}

return M

