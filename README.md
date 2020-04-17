# Bore
The most boring front-end build tool. Generates build files for back-end build
tools such as make or ninja.

## Usage
```
Bore - A boring build file generator.

Consumes human-writable rule modules written in Lua, and generates corresponding
build files for back-end build tools such as Make and Ninja.

Usage: bore [--option] [--generator-options] <generator> [<generator>...]

Options:
    -h, --help                      Print this message and exit.

    -c <dir>, --cwd <dir>           The root directory of the project. Unless the -b option is provided,
                                    this directory should contain a file build.lua (default is the current
                                    working directory).

    -i <file>                       The build file to use for the project (default is build.lua)

    --no-clean                      Prevent the generation of an automatic clean target

    -o <dir>, --build-dir <dir>     The out-of-source build directory to use for out of source
                                    build rules (default is the /build relative to the root directory of
                                    the project).

Generator Specific Options:
    --make-file <file>              The output Makefile (default is Makefile).

    --ninja-file <file>             The output ninja file (default is ninja.build).

    --graph-file <file>             The output graph file (default is dependencies.dot).

Generators:
    --make                          Generates a Makefile for use with make. Will create a single Makefile
                                    in the project root.

    --ninja                         Generates a single ninja.build file for use with the Ninja build
                                    tool.

    --graph                         Generates a DOT graph file to describe the dependency graph.

```

## TODO
 * Custom CLI parser?
 * Group rules (replacing for loops)?
 * Directory management
 * Automagic build regen
 * Ninja generation
 * Pre-defined rules (c, c++, asm, etc)
 * Parameter configuration?
 * Documentation
 * Examples

