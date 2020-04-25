
function usage()
    print("Usage: lua runner.lua <DIR>...")
end

function run(test)
    return os.execute("../../bin/bore --dry-run -b /tmp -C " .. test)
end

if #arg == 0 then
    usage()
    os.exit(1)
end

print("-------- Common Tests --------")

for i = 1, #arg, 1 do
    print("Running: " .. arg[i] .. " ...")
    if run(arg[i]) ~= true then
        print()
        print("Test failed!")
        os.exit(2)
    end
end

print()
print("All tests passed!")

os.exit(0)

