
local files = module.glob(path.join("files", "*.txt"))

if #files ~= 4 then
    error("Glob expected 4 files, but only got " .. #files)
end

for _, f in pairs(files) do
    if path.extension(f) ~= "txt" then
        error("Expected .txt file matched by glob, but file was: " .. f)
    end
end

