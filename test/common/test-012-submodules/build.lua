
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

submodule(module, module.path("other", "build.lua"))

target {
    name = "all",
    build = rule {
        ins = {},
        outs = {},
        cmds = "${other.outs}"
    }
}

assert_equals(targets.all.cmds[1], module.path("other", "hello.txt"))

