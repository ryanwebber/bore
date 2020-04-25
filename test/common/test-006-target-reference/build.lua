
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

target {
    name = "test",
    build = rule {
        ins = { "a", "aa", "aaa" },
        outs = "b",
        cmds = "c",
        dirs = "d"
    }
}

assert_equals(targets.test.ins[1], "a" )
assert_equals(targets.test.ins[2], "aa" )
assert_equals(targets.test.ins[3], "aaa" )
assert_equals(targets.test.outs[1], "b" )
assert_equals(targets.test.cmds[1], "c" )
assert_equals(targets.test.dirs[1], "d" )

