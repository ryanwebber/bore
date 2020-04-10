local M = {
    name = "greeting",
    targets = {}
}

M.targets.main = rule {
    ins = build.modules.libGreeting.main.outs,
    outs = { "transcript" },
    cmds = {
        "cat %{ins} %{outs}"
    }
}

return M;

