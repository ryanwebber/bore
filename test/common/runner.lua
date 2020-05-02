
function usage()
    print("Usage: lua runner.lua <DIR>...")
end

function run(test)
    local test_dir = test
    if test_dir:sub(-1) ~= "/" then
        test_dir = test_dir .. "/"
    end

    return os.execute("../../bin/bore --dry-run -b /tmp -f " .. test_dir .. "bore.lua")
end

if #arg == 0 then
    usage()
    os.exit(1)
end

print("-------- Common Tests --------")

for i = 1, #arg, 1 do
    print("Running: " .. arg[i])
    if run(arg[i]) ~= true then
        print()
        print("Test failed!")
        os.exit(2)
    end
end

print()
print("All tests passed!")

os.exit(0)

