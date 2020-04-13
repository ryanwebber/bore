
local name = "Ryan webber"
local build_dir = module.local_build_dir

target {
    name = "template",
    build = rule {
        ins = {},
        outs = { path.join(build_dir, "template.txt") },
        cmds = { string.format("echo \"Hello %s\" > template.txt", name) }
    }
}

