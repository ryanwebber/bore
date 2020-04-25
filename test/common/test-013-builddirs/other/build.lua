
target {
    name = "test2",
    build = rule {
        ins = {},
        outs = module.object("b"),
        cmds = {},
    }
}

target {
    name = "test3",
    build = rule {
        ins = {},
        outs = path.join(module.root_build_dir, "c"),
        cmds = {}
    }
}

