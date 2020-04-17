#include <iostream>
#include <string>

#include "runtime.h"
#include "argopts.h"
#include "configuration_exception.h"
#include "generation_exception.h"

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
            "Consumes human-writable rule modules written in Lua, "
            "and generates corresponding build files for back-end "
            "build tools such as Make and Ninja."
            "\n"
            "Bore will evaluate a top level lua file that will define "
            "the targets of this project."
            ); 

    program.add_epilog(
            "\n"
            "Supported Generators:\n"
            "    --make         Makefiles for use with make\n"
            "    --ninja        Ninja build files for use with ninja\n"
            "    --graph        Dependency graphs using DOT notation\n"
            );

    program.add_argument("-b", "--build-file")
        .help("the root lua build descriptor file (defaults to build.lua).")
        .default_value(std::string("build.lua"));

    program.add_argument("-o", "--objects")
        .help("the main build folder for storing temporary build files (defaults to build/).")
        .default_value(std::string("build/"));

    program.add_argument("-v", "--verbose")
        .help("output verbose logs (defaults to false).")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--graph")
        .help("output a dot graph file describing the dependency graph")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--make")
        .help("output a Makefile for use with make")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--ninja")
        .help("output a ninja build file for use with ninja")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--make-output")
        .help("the Makefile to create when using the make generator  (defaults to Makefile)")
        .default_value(std::string("Makefile"));

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        return 1;
    }

    ArgOpts opts(program);

    Runtime runtime;
    RuntimeConfiguration conf = {
        .build_dir = opts.get<std::string>("--objects")
    };

    std::unique_ptr<BuildGraph> graph;

    try {
        graph = runtime.loadAndEvaluate(opts.get("--build-file"), conf);
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

    try {
        for (auto g : generators) {
            g->generate(*graph, opts);
        }
    } catch(GenerationException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

