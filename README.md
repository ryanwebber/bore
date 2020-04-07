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

## Mental Map
C++ Stuff:
 - Implementation of defnrule (**could be done in lua**)
   - Needs to update the lua runtime with a new global function
   - When run, this global function needs to:
     - Be invoked with a table 
     - Validate the table against the rule spec
     - Return an array of build step tree nodes (build step = table like { ins, outs, cmds })
 - Implementation of basic rule (** could be in Lua **)
   - Basic rule is just a function (exactly like above) that
     - Accepts a build step table as it's input
     - Returns the build step table as it's output
   - Basic rule fn is put in the global as a function
 - Utility functions (** could be in Lua **)
    - Path stuff
 - Templating and variable substitution

Lua Stuff:
 - Basic rules (ex cexecutable, clib)

Notes:
 - A rule is just a function that consumes a table and returns build step tree
 nodes (an array of child nodes and a build step ({ins, outs, cmds})
 - Defnrule is just a function that consumes a name and a table, and adds a new rule function
 to the global scope. This rule function should be in c++ 

## Getting Started
_More documentation to come_

