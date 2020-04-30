
if type(env.local_dir) ~= "string" then
    error("env.local_dir was not a string")
end

if type(env.build_dir) ~= "string" then
    error("env.build_dir was not a string")
end

env.path("a", "b")
env.object()

