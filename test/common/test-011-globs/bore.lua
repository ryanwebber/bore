
local files = env.glob(path.join("files", "*.txt"))

if #files ~= 4 then
    error("Glob expected 4 files, but only got " .. #files)
end

local expected = {
    ["files/d.e.txt"] = 1,
    ["files/c.txt"] = 1,
    ["files/b.txt"] = 1,
    ["files/a.txt"] = 1
}

for _, f in pairs(files) do
    if not expected[f] then
        error("Unexpected file matched by glob: " .. f)
    end
end

