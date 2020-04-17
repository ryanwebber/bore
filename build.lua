
local cc = "g++-9"
local cflags = "-g -std=c++17 -Wall"
local include = "-I include"
local lib = "-llua -lm -ldl"

local obj_files = array.map(module.glob("src/*.cpp"), function (_, source)
    local t = target {
        name = source,
        build = rule {
            ins = source,
            outs = module.object(path.basename(source) .. ".o"),
            cmds = {
                "mkdir -p " .. module.object(),
                table.concat({ cc, cflags, include, "-c", "-o", "${outs}", "${ins}" }, " ")
            }
        }
    }

    return t.outs
end)

target {
    name = "luaBundle",
    build = rule {
        ins = { module.path("pkg/core.lua"), module.glob("pkg/**/*.lua") },
        outs = module.object("bundle.lua"),
        cmds = {
            "mkdir -p " .. module.object(),
            "cat ${ins} > ${outs}"
        }
    }
}

target {
    name = "luaEmbed",
    build = rule {
        ins = targets.luaBundle.outs,
        outs = module.object("__lua_embed.o"),
        cmds = {
            "mkdir -p " .. module.object(),
            "ld -r -b binary -o ${outs} ${ins}"
        }
    }
}

target {
    name = "bore",
    build = rule {
        ins = { obj_files, "${luaEmbed.outs}" },
        outs = module.path(path.join("bin", "bore2")),
        cmds = {
            "mkdir -p " .. module.path("bin/"),
            cc .. " ${ins} -o ${outs} " .. lib
        }
    }
}

target {
    name = "all",
    build = phony {
        deps = { targets.bore }
    }
}
