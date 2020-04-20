#include <string.h>

#include "argscan.h"
#include "graph_generator.h"
#include "lua_runtime.h"
#include "make_generator.h"
#include "path.h"

static void usage() {
    const char* u = ""
        "Usage:\n"
        "    bore [--options] --make [--make-makefile]\n"
        "    bore [--options] --ninja\n"
        "    bore [--options] --graph\n"
        "\n"
        "Consumes human-writable rule modules written in Lua, and generates corresponding "
        "build files for back-end build tools such as Make and Ninja. "
        "Bore will evaluate a top level lua file that will define the "
        "targets of this project.\n"
        "\n"
        "Optional arguments:\n"
        "    -h,--help                 show this help message and exit.\n"
        "    -b,--build-dir <DIR>      the out-of-source build folder files (defaults to build/).\n"
        "    -C,--directory <DIR>      the root project directory (defaults to $CWD).\n"
        "    -f,--file <FILE>          the root lua build descriptor file (defaults to $CWD/build.lua).\n"
        "    -v --verbose              output verbose logs (defaults to false).\n"
        "\n"
        "Supported generators:\n"
        "    --make                    Makefiles for use with make.\n"
        "    --ninja                   Ninja build files for use with ninja.\n"
        "    --graph                   dependency graphs using DOT notation.\n"
        "\n"
        "Generator-specific options:\n"
        "    --make-makefile <FILE>    the path of the Makefile to generate.\n"
        ;

    fprintf(stderr, "%s\n", u);
}

enum GeneratorType {
    t_make,
    t_ninja,
    t_graph
};

struct Program {
    const char* project_root;
    const char* build_file;
    const char* build_dir;
    enum GeneratorType generator_type;

    union {
        struct MakeOpts make;
    } u;
};

static struct Program p = {
    .project_root = "",
    .build_file = NULL,
    .build_dir = "build",
    .generator_type = t_make,
    .u = {
        .make = {
            .makefile = "Makefile"
        }
    }
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

static void opt_make(const char* argp[], int argc) {
    if (p.generator_type != t_make) {
        p.generator_type = t_make;
        p.u.make.makefile = "Makefile";
    }
}

static void opt_make_makefile(const char* argp[], int argc) {
    if (p.generator_type != t_make) {
        p.generator_type = t_make;
    }

    p.u.make.makefile = argp[1];
}

static void opt_graph(const char* argp[], int argc) {
    if (p.generator_type != t_graph) {
        p.generator_type = t_graph;
    }
}

static struct ArgHandler arguments[] = {

    // Regular options
    { "-h"              , 0, opt_help               },
    { "--help"          , 0, opt_help               },

    { "-b"              , 1, opt_build_dir          },
    { "--build-dir"     , 1, opt_build_dir          },

    { "-C"              , 1, opt_project_root       },
    { "--directory"     , 1, opt_project_root       },

    { "-f"              , 1, opt_build_file         },
    { "--file"          , 1, opt_build_file         },

    // Make options
    { "--make"          , 0, opt_make               },
    { "--make-makefile" , 1, opt_make_makefile      },

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

    if (p.build_file == NULL) {
        // Create the buffer with some arbitrary padding for the path separator
        size_t len = strlen(p.project_root) + 32;
        char buf[len];
        path_join(p.project_root, "build.lua", buf, len);
        p.build_file = strndup(buf, len);
    }

    struct BuildGraph graph;
    graph_init(&graph);

    struct LuaRuntime runtime;
    runtime_init(&runtime);
    runtime_evaluate(&runtime, p.project_root, p.build_dir, p.build_file, &graph, &err);
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
            graph_generate(&graph);
            break;
        default:
            fprintf(stderr, "Generator is currently not supported. Ignoring...\n");
            break;
    }

    if (err != NULL) {
        fprintf(stderr, "Error: %s\n", err->msg);
        return 4;
    }

    graph_free(&graph);

    return 0;
}

