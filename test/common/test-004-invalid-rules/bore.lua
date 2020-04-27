
function assert_error(f)
    local result, _ = pcall(f)
    if result ~= false then
        error("Function completed without error", 2)
    end
end

assert_error(function()
    rule ""
end)

assert_error(function()
    rule(nil)
end)

assert_error(function()
    rule(false)
end)

