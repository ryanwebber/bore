# Bore
The most boring front-end build tool. Generates build files for back-end build
tools such as make or ninja.

## Usage
```
Usage: bore [--option] [--generator-option] <generator> [<generator>...]

Consumes human-writable rule modules written in Lua, and generates corresponding build files for back-end build tools such as Make and Ninja.
Bore will evaluate a top level lua file that will define the targets of this project.

Optional arguments:
    -h --help       	show this help message and exit
    -b --build-file 	the root lua build descriptor file (defaults to build.lua).
    -o --objects    	the main build folder for storing temporary build files (defaults to build/).
    -v --verbose    	output verbose logs (defaults to false).
    --graph         	output a dot graph file describing the dependency graph
    --make          	output a Makefile for use with make
    --ninja         	output a ninja build file for use with ninja
    --make-output   	the Makefile to create when using the make generator  (defaults to Makefile)

Supported Generators:
    --make         Makefiles for use with make
    --ninja        Ninja build files for use with ninja
    --graph        Dependency graphs using DOT notation
```

## TODO
 * Ninja generation
 * Rules (c, c++, asm, etc)
 * Parameter configuration?
 * Documentation
 * Examples

