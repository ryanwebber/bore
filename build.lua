
local cc = "g++-9"
local cflags = "-g -std=c++17 -Wall"
local include = "-I include"
local lib = "-llua -lm -ldl"

local bin = module.path("bin")
local build = module.object()

local prefix = "/usr/local"

local obj_files = array.map(module.glob("src/*.cpp"), function (i, source)
    local t = target {
        name = source,
        build = rule {
            ins = source,
            outs = module.object(path.basename(source) .. ".o"),
            cmds = {
                table.concat({ cc, cflags, include, "-c", "-o", "${outs}", "${ins}" }, " ")
            }
        }
    }

    return i, t.outs
end)

target {
    name = "luaBundle",
    build = rule {
        ins = { module.path("pkg/core.lua"), module.glob("pkg/**/*.lua") },
        outs = module.object("bundle.lua"),
        cmds = {
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
            "ld -r -b binary -o ${outs} ${ins}"
        }
    }
}

target {
    name = "bore",
    build = rule {
        ins = { obj_files, targets.luaEmbed.outs },
        outs = path.join(bin, "bore2"),
        cmds = {
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

target {
    name = "install",
    build = rule {
        ins = targets.bore.outs,
        outs = {},
        cmds = {
            "install -d " .. path.join(prefix, "bin"),
            "install -C ${ins} " .. path.join(prefix, "bin")
        }
    }
}

target {
    name = "clean",
    build = rule {
        ins = {},
        outs = {},
        cmds = {
            "rm -r " .. bin,
            "rm -r " .. build
        }
    }
}

target {
    name = "test",
    build = rule {
        ins = {},
        outs = {},
        cmds = {
            string.format("%s -r %s -t /tmp/bore/maketest",
                module.path("test", "sanity", "maketest"),
                module.path("."))
        }
    }
}

