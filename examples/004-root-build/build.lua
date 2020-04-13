
-- Similar to require, but sets up the available
-- build paths correctly
submodule(module, "templating/build.lua")

target {
    name = "greet",
    build = rule {
        ins = { path.join(module.local_dir, "${template.ins}") },
        outs = { },
        cmds = { "cat ${ins}" }
    }
}

target {
    name = "test1",
    build = rule {
        ins = { "one", "two" },
        outs = {"three", "four" },
        cmds = { "five", "six" }
    }
}

target {
    name = "test2",
    build = rule {
        ins = targets.test1.outs,
        outs = "fake",
        cmds = { "echo hi" }
    }
}

target {
    name = "test3",
    build = rule {
        ins = { "fake", "four" },
        outs = {},
        cmds = {"echo hi"}
    }
}

target {
    name = "all",
    build = phony {
        deps = { targets.greet },
        cmds = {}
    }
}

