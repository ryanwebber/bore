
local cc = "g++-9"
local cflags = "-g -std=c++17 -Wall"
local include = "-I include"
local lib = "-llua -lm -ldl"

local obj_files = {}

target {
    name = "luaBundle",
    build = rule {
        ins = { module.path("pkg/core.lua"), module.glob("pkg/**/*.lua") },
        outs = module.object("bundle.lua"),
        cmds = "cat ${ins} > ${outs}"
    }
}

target {
    name = "luaEmbed",
    build = rule {
        ins = targets.luaBundle.outs,
        outs = module.object("__lua_embed.o"),
        cmds = "ld -r -b binary -o ${outs} ${ins}"
    }
}

for _, source in pairs(module.glob("src/*.cpp")) do
    target {
        name = source,
        build = rule {
            ins = source,
            outs = module.object(path.basename(source) .. ".o"),
            cmds = table.concat({
                cc,
                cflags,
                include,
                "-c",
                "-o",
                "${outs}",
                "${ins}",
            }, " ")
        }
    }

    for _, v in pairs(targets[source].outs) do table.insert(obj_files, v) end
end

for _, v in pairs(targets.luaEmbed.outs) do table.insert(obj_files, v) end

target {
    name = "bore",
    build = rule {
        ins = obj_files,
        outs = module.path(path.join("bin", "bore2")),
        cmds = cc .. " ${ins} -o ${outs} " .. lib
    }
}

target {
    name = "all",
    build = phony {
        deps = { targets.bore }
    }
}

