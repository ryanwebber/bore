
if type(print) ~= "function" then
    error("Build template globals not loaded properly")
end

if path.basename(env.local_dir) ~= "other" then
    error("Module loaded at unexpected location: " .. env.local_dir)
end

target {
    name = "other",
    build = rule {
        ins = {},
        outs = env.path("hello.txt"),
        cmds = {}
    }
}

