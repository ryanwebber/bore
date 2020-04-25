
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end

defnrule("test", {
    validator = function(args)
        return {
            thing2 = args.thing .. " y"
        }
    end,
    generator = function(args)
        return rule {
            ins = {},
            outs = {},
            cmds = args.thing2
        }
    end
})

target {
    name = "abc",
    build = test {
        thing = "x"
    }
}

assert_equals(targets.abc.cmds[1], "x y")

