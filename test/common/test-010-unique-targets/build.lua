
function assert_error(f)
    local result, _ = pcall(f)
    if result ~= false then
        error("Function completed without error", 2)
    end
end

target {
    name = "abc",
    build = rule {
        ins = {},
        outs = {},
        cmds = {}
    }
}

assert_error(function()
    target {
        name = "abc",
        build = rule {
            ins = {},
            outs = {},
            cmds = {},
        }
    }
end)

