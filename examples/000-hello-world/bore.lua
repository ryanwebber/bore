target {
    name = "hello",
    default = true,
    build = rule {
        cmds = "echo Hello World!"
    }
}

