
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

include("other/build.lua")

target {
    name = "test1",
    build = rule {
        ins = {},
        outs = env.object("a"),
        cmds = {},
    }
}

assert_equals(targets.test1.outs[1], "/tmp/a")
assert_equals(targets.test2.outs[1], "/tmp/b")
assert_equals(targets.test3.outs[1], "/tmp/c")

