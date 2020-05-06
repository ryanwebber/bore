
local cc = "gcc"
local cflags = "-g -Wall"
local include = "-I include"
local lib = "-llua -lm -ldl"

local bin = env.path("bin")
local build = env.object()

local prefix = type(config.prefix) == "string" and config.prefix or "/usr/local"

local obj_files = {}
for _, source in pairs(env.glob("src/*.c")) do

    local t = target {
        name = "c_" .. source,
        alias = false,
        build = c.obj {
            sources = source,
            flags = { "-g", "-Wall", "-I " .. env.path("include") },
            build_dir = env.object(),
        }
    }

    table.insert(obj_files, t.outs[1])
end

target {
    name = "luaBundle",
    alias = false,
    build = rule {
        ins = { env.path("pkg/core.lua"), env.glob("pkg/**/*.lua") },
        outs = env.object("bundle.lua"),
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
        outs = env.object("__lua_embed.o"),
        cmds = {
            "ld -r -b binary -o ${outs} ${ins}"
        }
    }
}

target {
    name = "bore",
    default = true,
    description = "Build the Bore executable",
    build = c.executable {
        objects = { obj_files, targets.luaEmbed.outs },
        binary = "bore",
        libs = { "-llua", "-lm", "-ldl" },
        bin_dir = env.path("bin"),
    }
}

target {
    name = "install",
    description = "Install Bore to " .. prefix,
    phony = true,
    build = rule {
        ins = targets.bore.outs,
        cmds = {
            "install -d " .. path.join(prefix, "bin"),
            "install -C ${ins} " .. path.join(prefix, "bin")
        }
    }
}

target {
    name = "clean",
    description = "Remove all build artifacts",
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
    description = "Run the common test suite",
    phony = true,
    build = rule {
        ins = targets.bore.name,
        cmds = "cd test/common && lua runner.lua test-*"
    }
}

target {
    name = "sanitytest",
    description = "Run the sanity tests",
    phony = true,
    build = rule {
        cmds = {
            "test/sanity/maketest -r " .. env.path(".") .. " -t /tmp/bore/maketest",
            "test/sanity/ninjatest -r " .. env.path(".") .. " -t /tmp/bore/ninjatest",
        }
    }
}

target {
    name = "test",
    description = "Run all tests",
    phony = true,
    build = rule {
        ins = { targets.commontest.name, targets.sanitytest.name }
    }
}

