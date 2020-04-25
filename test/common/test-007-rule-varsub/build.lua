
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

target {
    name = "test1",
    build = rule {
        ins = { "a", "b" },
        outs = { "c", "d" },
        cmds = "${ins} + ${outs}",
    }
}

target {
    name = "test2",
    build = rule {
        ins = {},
        outs = {},
        cmds = "${ins} + ${outs}"
    }
}

target {
    name = "test3",
    build = rule {
        ins = {},
        outs = {},
        cmds = "${test1.ins} + ${test1.outs}"
    }
}


assert_equals(targets.test1.cmds[1], "a b + c d")
assert_equals(targets.test2.cmds[1], " + ")
assert_equals(targets.test3.cmds[1], "a b + c d")

