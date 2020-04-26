
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

target {
    name = "test1",
    build = rule {
        ins = {},
        outs = {},
        cmds = {},
        dirs = "dir"
    }
}

target {
    name = "test2",
    build = rule {
        ins = {},
        outs = {},
        cmds = {},
        dirs = { "a", "b" }
    }
}

target {
    name = "test3",
    build = rule {
        ins = {},
        outs = { "a/b/c/d.txt", "x/y/z.a" },
        cmds = {}
    }
}

target {
    name = "test4",
    build = rule {
        ins = {},
        outs = { "x.txt" },
        cmds = {}
    }
}

assert_equals(targets.test1.dirs[1], "dir")
assert_equals(targets.test2.dirs[1], "a")
assert_equals(targets.test2.dirs[2], "b")
assert_equals(targets.test3.dirs[1], "a/b/c/")
assert_equals(targets.test3.dirs[2], "x/y/")
assert_equals(targets.test4.dirs[1], nil)

