local M = {
    name = "greeting",
    targets = {}
}

M.targets.main = rule {
    ins = { "greeting.txt" },
    outs = { "transcript.txt" },
    cmds = {
        "cat %{ins} > %{outs}"
    }
}

return M;

