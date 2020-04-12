
-- Similar to require, but sets up the global
-- build variable
submodule("myLib/build.lia")

target {
    name "greet",
    build = rule {
        ins = { "${template.ins}" }, -- could also use targets.template.ins
        outs = { },
        cmds = { "cat ${ins}" }
    }
}

target {
    name = "all",
    build = phony {
        deps = { targets.greet },
        cmds = {}
    }
}

