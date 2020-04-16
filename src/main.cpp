#include <iostream>
#include <string>

#include "runtime.h"
#include "argopts.h"
#include "configuration_exception.h"

#include "vendor/argparse.hpp"

#include "graph_generator.h"
#include "make_generator.h"

int main(int argc, const char* argv[]) {

    argparse::ArgumentParser program("bore");
    program.add_usage(
            "\n"
            "Usage: bore [--option] [--generator-option] <generator> [<generator>...]"
            );

    program.add_description(
            "Bore - A boring build file generator."
            "\n\n"
            "Consumes human-writable rule modules written in Lua, "
            "and generates corresponding build files for back-end "
            "build tools such as Make and Ninja."
            "\n\n"
            "Bore will evaluate a top level lua file that will define "
            "the targets of this project."
            ); 

    program.add_epilog(
            "\n"
            "Supported Generators:\n"
            " * Makefiles for use with make (via --make)\n"
            " * Ninja build files for use with ninja (via --ninja)\n"
            " * Dependency graphs using DOT notation (via --graph)\n"
            );

    program.add_argument("-b", "--build-file")
        .help("The root lua build descriptor file (defaults to build.lua).")
        .default_value(std::string("build.lua"));

     program.add_argument("--no-clean")
        .help("Don't generate an automatic clean target (enabled by default).")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--objects")
        .help("The main build folder for storing temporary build files (defaults to build/).");

    program.add_argument("-v", "--verbose")
        .help("Output verbose logs (defaults to false).")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--graph")
        .help("Output a dot graph file describing the dependency graph")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--make")
        .help("Output a Makefile for use with make")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--ninja")
        .help("Output a Makefile for use with make")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        return 1;
    }

    ArgOpts opts(program);

    Runtime runtime;
    std::unique_ptr<BuildGraph> graph;

    try {
        graph = runtime.loadAndEvaluate(opts.get("--build-file"));
    } catch (ConfigurationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<std::shared_ptr<Generator>> generators;
    if (opts.get<bool>("--make"))
        generators.push_back(std::make_shared<MakeGenerator>());
 
    if (opts.get<bool>("--graph"))
        generators.push_back(std::make_shared<GraphGenerator>());

    if (generators.empty())
        generators.push_back(std::make_shared<MakeGenerator>());

    for (auto g : generators) {
        g->generate(*graph, opts);
    }

    return 0;
}

