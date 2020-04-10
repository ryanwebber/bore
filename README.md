# Bore
The most boring front-end build tool. Generates build files for back-end build
tools such as make or ninja.

## Usage
```
Bore - A boring build file generator.

Consumes human-writable rule modules written in Lua, and generates corresponding
build files for back-end build tools such as Make and Ninja.

Usage:
    bore [--options] [<build-scripts>...]

Arguments:
    Takes a list of lua scripts that define new rules or return modules containing
    build targets. Scripts are evaluated in-order, so scripts that define new
    rules should come prior to the scripts that define build targets using
    those rules.

Options:
    -r --root-dir   The root build directory. Assumed to be the current working
                    directory. If the root build directory does not have a
                    build.lua file in it, the generator will print an error and exit.

    -o --build-dir  The out-of-source build directory to use for out of source
                    build rules. Defaults to <root-dir>/build.

```

## TODO
 * Rules should be userdata
 * Module parsing should be done in lua
   * Checks for duplicate module definitions, output file collisions,
     cyclical dependencies, etc
   * Module representation should stay as c++ though (using userdata)

## Getting Started
_More documentation to come_

