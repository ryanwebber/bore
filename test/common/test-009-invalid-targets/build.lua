
function assert_error(f)
    local result, _ = pcall(f)
    if result ~= false then
        error("Function completed without error", 2)
    end
end

assert_error(function()
    target {
    }
end)

assert_error(function()
    target {
        name = "abc",
    }
end)

assert_error(function()
    target {
        build = rule {
            ins = {},
            outs = {},
            cms = {}
        }
    }
end)

assert_error(function()
    target(nil)
end)

assert_error(function()
    target {
        name = "abcdef",
        build = {
            ins = {},
            outs = {},
            cmds = {},
        }
    }
end)

assert_error(function()
    target {
        name = "phony",
        phony = true,
        build = rule {
            outs = "a"
        }
    }
end)

