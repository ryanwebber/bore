
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
        outs = path.join(module.build_dir, "c"),
        cmds = {}
    }
}

