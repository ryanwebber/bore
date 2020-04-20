# Bore
The most boring front-end build tool. Generates build files for back-end build
tools such as make or ninja.

## Usage
```
Usage: bore [--option] <generator-type> [--generator-option]

Consumes human-writable rule modules written in Lua, and generates corresponding build files for back-end build tools such as Make and Ninja. Bore will evaluate a top level lua file that will define the targets of this project.

Optional arguments:
    -h,--help                 show this help message and exit
    -b,--build-dir <DIR>      the main build folder for storing temporary build files (defaults to build/).
    -C,--directory <DIR>      the root project directory (defaults to $CWD).
    -f,--file <FILE>          the root lua build descriptor file (defaults to $CWD/build.lua).
    -v --verbose              output verbose logs (defaults to false).

Supported generators:
    --make                    Makefiles for use with make
    --ninja                   Ninja build files for use with ninja
    --graph                   dependency graphs using DOT notation

Generator-specific options:
    --make-makefile <FILE>    the path of the Makefile to generate

```

## TODO
 * Ninja generation
 * Rules (c, c++, asm, etc)
 * Parameter configuration?
 * Documentation
 * Examples

