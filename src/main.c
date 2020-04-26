#include <string.h>

#include "argscan.h"
#include "gen_dot.h"
#include "gen_make.h"
#include "gen_ninja.h"
#include "lua_runtime.h"
#include "path.h"

#define VERSION "0.1.0"

static void usage() {
    const char* u = ""
        "Usage:\n"
        "    bore [--options] --make [--make-file]\n"
        "    bore [--options] --ninja [--ninja-file]\n"
        "    bore [--options] --graph\n"
        "    bore [--options] --dry-run\n"
        "    bore -h | --help\n"
        "    bore --version\n"
        "\n"
        "Consumes human-writable rule modules written in Lua, and generates corresponding "
        "build files for back-end build tools such as Make and Ninja. "
        "Bore will evaluate a top level lua file that will define the "
        "targets of this project.\n"
        "\n"
        "Optional arguments:\n"
        "    -h,--help                 show this help message and exit.\n"
        "    -b,--build-dir <DIR>      the out-of-source build folder files (defaults to build/).\n"
        "    -C,--directory <DIR>      the root project directory (defaults to the current directory).\n"
        "    --config <KEY> <VALUE>    assign a key-value pair to the global lua config table.\n"
        "    --dry-run                 attempt to parse the build file, but don't generate anything\n"
        "    -f,--file <FILE>          the root lua build descriptor file (defaults to build.lua).\n"
        "    -v --verbose              output verbose logs (defaults to false).\n"
        "    --version                 print the version and exit\n"
        "\n"
        "Supported generators:\n"
        "    --make                    Makefiles for use with make.\n"
        "    --ninja                   Ninja build files for use with ninja.\n"
        "    --graph                   dependency graphs using DOT notation.\n"
        "\n"
        "Generator-specific options:\n"
        "    --make-file <FILE>        the path of the Makefile to generate.\n"
        "    --ninja-file <FILE>       the path of the Ninja build file to generate.\n"
        ;

    fprintf(stderr, "%s\n", u);
}

enum GeneratorType {
    t_none,
    t_make,
    t_ninja,
    t_graph,
    t_dry
};

struct Program {
    const char* project_root;
    const char* build_file;
    const char* build_dir;
    struct KeyValueList config;
    enum GeneratorType generator_type;

    union {
        struct MakeOpts make;
        struct NinjaOpts ninja;
    } u;
};

static struct Program p = {
    .project_root = "",
    .build_file = "build.lua",
    .build_dir = "build",
    .config = { NULL },
    .generator_type = t_none,
};

static void opt_help(const char* argp[], int argc) {
    usage();
    exit(1);
}

static void opt_build_dir(const char* argp[], int argc) {
    p.build_dir = argp[1];
}

static void opt_project_root(const char* argp[], int argc) {
    p.project_root = argp[1];
}

static void opt_build_file(const char* argp[], int argc) {
    p.build_file = argp[1];
}

static void opt_config(const char* argp[], int argc) {
    kvp_add(&p.config, argp[1], argp[2]);
}

static void opt_version(const char* argp[], int argc) {
    fprintf(stderr, "bore v" VERSION "\n");
    exit(1);
}

static void opt_make(const char* argp[], int argc) {
    if (p.generator_type != t_make && p.generator_type != t_dry) {
        p.generator_type = t_make;
        p.u.make.makefile = "Makefile";
    }
}

static void opt_make_file(const char* argp[], int argc) {
    if (p.generator_type != t_dry) {
        p.generator_type = t_make;
    }

    p.u.make.makefile = argp[1];
}

static void opt_ninja(const char* argp[], int argc) {
    if (p.generator_type != t_ninja && p.generator_type != t_dry) {
        p.generator_type = t_ninja;
        p.u.make.makefile = "build.ninja";
    }
}

static void opt_ninja_file(const char* argp[], int argc) {
    if (p.generator_type != t_dry) {
        p.generator_type = t_ninja;
    }

    p.u.ninja.ninja_file = argp[1];
}

static void opt_graph(const char* argp[], int argc) {
    if (p.generator_type != t_dry)
        p.generator_type = t_graph;
}

static void opt_dry(const char* argp[], int argc) {
    p.generator_type = t_dry;
}

static struct ArgHandler arguments[] = {

    // Regular options
    { "-h"              , 0, opt_help               },
    { "--help"          , 0, opt_help               },

    { "-b"              , 1, opt_build_dir          },
    { "--build-dir"     , 1, opt_build_dir          },

    { "-C"              , 1, opt_project_root       },
    { "--directory"     , 1, opt_project_root       },

    { "--config"        , 2, opt_config             },

    { "--dry-run"       , 0, opt_dry                },

    { "-f"              , 1, opt_build_file         },
    { "--file"          , 1, opt_build_file         },

    { "--version"       , 0, opt_version            },

    // Make options
    { "--make"          , 0, opt_make               },
    { "--make-file"     , 1, opt_make_file          },

    // Ninja options
    { "--ninja"         , 0, opt_ninja              },
    { "--ninja-file"    , 1, opt_ninja_file         },

    // Graph options
    { "--graph"         , 0, opt_graph              },

    // Sentinal
    { NULL              , 0, NULL                   }
};

int main(int argc, const char* argv[]) {

    struct Error *err = NULL;

    argscan(arguments, argc, argv, &err);
    if (err != NULL) {
        fprintf(stderr, "Error: %s\n\n", err->msg);
        usage();
        return 2;
    }

    struct BuildGraph graph;
    graph_init(&graph);

    struct LuaRuntime runtime;
    runtime_init(&runtime);
    runtime_evaluate(&runtime, p.project_root, p.build_dir, p.build_file, &p.config, &graph, &err);
    runtime_free(&runtime);

    if (err != NULL) {
        fprintf(stderr, "%s\n", err->msg);
        return 3;
    }

    switch (p.generator_type) {
        case t_make:
            make_generate(&graph, &p.u.make, &err);
            break;
        case t_graph:
            dot_generate(&graph);
            break;
        case t_ninja:
            ninja_generate(&graph, &p.u.ninja, &err);
        case t_dry:
            break;
        case t_none:
            fprintf(stderr, "Error: The generator type must be specified\n\n");
            usage();
            return 5;
    }

    if (err != NULL) {
        fprintf(stderr, "Error: %s\n", err->msg);
        return 4;
    }

    graph_free(&graph);
    kvp_free(&p.config);

    return 0;
}

