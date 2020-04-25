
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

submodule(module, module.path("other", "build.lua"))

target {
    name = "test1",
    build = rule {
        ins = {},
        outs = module.object("a"),
        cmds = {},
    }
}

assert_equals(targets.test1.outs[1], "/tmp/test-013-builddirs/a")
assert_equals(targets.test2.outs[1], "/tmp/test-013-builddirs/other/b")
assert_equals(targets.test3.outs[1], "/tmp/test-013-builddirs/c")

