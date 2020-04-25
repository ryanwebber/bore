
if type(module.local_dir) ~= "string" then
    error("module.local_dir was not a string")
end

if type(module.local_build_dir) ~= "string" then
    error("module.build_dir was not a string")
end

module.path("a", "b")
module.object()

