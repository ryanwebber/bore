
local program = "hello"

target {
    name = "hello_obj",
    alias = false,
    build = c.obj {
        sources = "src/main.c",
        build_dir = env.build_dir
    }
}

target {
    name = program,
    default = true,
    build = c.executable {
        objects = targets.hello_obj.outs,
        binary = program,
        bin_dir = "bin"
    }
}

