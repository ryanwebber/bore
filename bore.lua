
local cc = "gcc"
local cflags = "-g -Wall"
local include = "-I include"
local lib = "-llua -lm -ldl"

local bin = module.path("bin")
local build = module.object()

local prefix = type(config.prefix) == "string" and config.prefix or "/usr/local"

local obj_files = {}
for _, source in pairs(module.glob("src/*.c")) do

    local t = target {
        name = "c_" .. source,
        alias = false,
        build = c_obj {
            sources = source,
            flags = { "-g", "-Wall", "-I " .. module.path("include") },
            build_dir = module.object(),
        }
    }

    table.insert(obj_files, t.outs[1])
end

target {
    name = "luaBundle",
    alias = false,
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
    alias = false,
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
    default = true,
    build = rule {
        ins = { obj_files, targets.luaEmbed.outs },
        outs = path.join(bin, "bore"),
        cmds = {
            cc .. " ${ins} -o ${outs} " .. lib
        }
    }
}

target {
    name = "install",
    phony = true,
    build = rule {
        ins = targets.bore.name,
        cmds = {
            "install -d " .. path.join(prefix, bin),
            "install -C ${ins} " .. path.join(prefix, bin)
        }
    }
}

target {
    name = "clean",
    phony = true,
    build = rule {
        cmds = {
            "rm -rf " .. bin,
            "rm -rf " .. build
        }
    }
}

target {
    name = "commontest",
    phony = true,
    build = rule {
        ins = targets.bore.name,
        cmds = "cd test/common && lua runner.lua test-*"
    }
}

target {
    name = "sanitytest",
    phony = true,
    build = rule {
        cmds = {
            "test/sanity/maketest -r " .. module.path(".") .. " -t /tmp/bore/maketest",
            "test/sanity/ninjatest -r " .. module.path(".") .. " -t /tmp/bore/ninjatest",
        }
    }
}

target {
    name = "test",
    phony = true,
    build = rule {
        ins = { targets.commontest.name, targets.sanitytest.name }
    }
}

