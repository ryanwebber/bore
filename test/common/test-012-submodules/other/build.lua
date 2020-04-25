
if type(print) ~= "function" then
    error("Submodule globals not loaded properly")
end

if path.basename(module.local_dir) ~= "other" then
    error("Module loaded at unexpected location: " .. module.local_dir)
end

target {
    name = "other",
    build = rule {
        ins = {},
        outs = module.path("hello.txt"),
        cmds = {}
    }
}

