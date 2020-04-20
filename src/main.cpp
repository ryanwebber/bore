#include <iostream>
#include <string>
#include <filesystem>

#include <string.h>

#include "runtime.h"
#include "argscan.h"
#include "configuration_exception.h"
#include "generation_exception.h"
#include "graph_generator.h"
#include "make_generator.h"

enum GeneratorType {
    t_make,
    t_ninja,
    t_graph
};

struct Program {
    const char* project_root;
    const char* build_file;
    const char* build_dir;
    GeneratorType generator_type;

    union {
        MakeOpts make;
    } u;
};

void usage() {
    std::cerr << std::endl
              << "Usage: " << std::endl
              << "    bore [--options] --make [--make-makefile]" << std::endl
              << "    bore [--options] --ninja" << std::endl
              << "    bore [--options] --graph" << std::endl
              << std::endl
              << "Consumes human-writable rule modules written in Lua, and generates corresponding "
              << "build files for back-end build tools such as Make and Ninja. "
              << "Bore will evaluate a top level lua file that will define the "
              << "targets of this project." << std::endl
              << std::endl
              << "Optional arguments:" << std::endl
              << "    -h,--help                 show this help message and exit" << std::endl
              << "    -b,--build-dir <DIR>      the main build folder for storing temporary build files (defaults to build/)." << std::endl
              << "    -C,--directory <DIR>      the root project directory (defaults to $CWD)." << std::endl
              << "    -f,--file <FILE>          the root lua build descriptor file (defaults to $CWD/build.lua)." << std::endl
              << "    -v --verbose              output verbose logs (defaults to false)." << std::endl
              << std::endl
              << "Supported generators:" << std::endl
              << "    --make                    Makefiles for use with make" << std::endl
              << "    --ninja                   Ninja build files for use with ninja" << std::endl
              << "    --graph                   dependency graphs using DOT notation" << std::endl
              << std::endl
              << "Generator-specific options:" << std::endl
              << "    --make-makefile <FILE>    the path of the Makefile to generate" << std::endl
              << std::endl;
}

int main(int argc, const char* argv[]) {

    Program p = {
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

    auto s = ArgScanner()
        .withOption("-h", "--help", 0, [](const char* arg[], int argc) {
            usage();
            exit(0);
        })
        .withOption("-b", "--build-dir", 1, [&p] (const char* arg[], int argc) mutable {
            p.build_dir = arg[1];
        })
        .withOption("-C", "--directory", 1, [&p] (const char* arg[], int argc) mutable {
            p.project_root = arg[1];
        })
        .withOption("-f", "--file", 1, [&p] (const char* arg[], int argc) mutable {
            p.build_file = arg[1];
        })
        .withOption("--make", 0, [&p] (const char* arg[], int argc) mutable {
            if (p.generator_type != t_make) {
                p.generator_type = t_make;
                p.u.make = {
                    .makefile = "Makefile"
                };
            }
        })
        .withOption("--make-makefile", 1, [&p] (const char* arg[], int argc) mutable {
            if (p.generator_type != t_make) {
                p.generator_type = t_make;
                p.u.make = {};
            }

            p.u.make.makefile = arg[1];
        })
        .withOption("--graph", 0, [&p] (const char* arg[], int argc) mutable {
            p.generator_type = t_graph;
        });


    try {
        s.scan(argc, argv);
    } catch(ArgScanException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        usage();
        return 2;
    }

    if (p.build_file == NULL) {
        auto f = std::filesystem::path(p.project_root) / "build.lua";
        p.build_file = strdup(f.c_str());
    }

    Runtime runtime;
    RuntimeConfiguration conf = {
        .root_dir = p.project_root,
        .build_dir = p.build_dir
    };

    std::unique_ptr<BuildGraph> graph;

    try {
        graph = runtime.loadAndEvaluate(p.build_file, conf);
    } catch (ConfigurationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    try {
        switch (p.generator_type) {
            case t_make:
                make_generate(*graph, p.u.make);
                break;
            case t_graph:
                graph_generate(*graph);
            default:
                std::cerr << "Generator not yet supported" << std::endl;
                break;
        }
    } catch(GenerationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

