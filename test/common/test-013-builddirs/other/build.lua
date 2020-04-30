
target {
    name = "test2",
    build = rule {
        ins = {},
        outs = env.object("b"),
        cmds = {},
    }
}

target {
    name = "test3",
    build = rule {
        ins = {},
        outs = path.join(env.build_dir, "c"),
        cmds = {}
    }
}

