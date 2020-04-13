
print("Do we have globals? " .. test)

-- Similar to require, but sets up the global
-- build variable
-- submodule("myLib/build.lua")

target {
    name = "greet",
    build = rule {
        ins = { "${template.ins}" }, -- could also use targets.template.ins
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
        ins = "three",
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

