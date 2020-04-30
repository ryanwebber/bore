---
layout: default
---

# Bore
A cross-platform build file generator for make and Ninja using templates written in Lua.

---

 * [Introduction](#introduction)
    * [About](#about)
    * [Features](#features)
 * [Getting Started](#getting-started)
    * [Installation](#installation)
    * [Running Bore](#running-bore)
    * [Generators](#generators)
        * [Caveats](#caveats)
 * [Build Templates](#build-templates)
    * [Defining Targets](#defining-targets)
    * [Creating Rules](#defining-rules)
    * [Environment](#environment)
 * [Examples](#)
 * [API Reference](#)
    * [Variables](#)
        * [`config`](#config)
        * [`env.build_dir`](#envbuild_dir)
        * [`env.local_dir`](#envlocal_dir)
        * [`env.root_dir`](#envroot_dir)
        * [`rules`](#rules)
        * [`targets`](#targets)

    * [Functions](#)
        * [`env.glob()`](#)
        * [`env.object()`](#)
        * [`env.path()`](#)
        * [`glob()`](#)
        * [`include(path)`](#)
        * [`path.basename()`](#)
        * [`path.dirname()`](#)
        * [`path.extension()`](#)
        * [`path.filename()`](#)
        * [`path.join()`](#)
        * [`rule()`](#ruleargs)
        * [`submodule()`](#submodulepath)
        * [`target()`](#targetargs)


## Introduction
Bore is a tool I wrote to ease my frustrations with other build systems for projects
that were small but required highly customized builds. I wanted build templates that were
as conceptually simple to write as Makefiles, but allowed me a little bit more flexibility
and modularity.

If you're considering Bore, consider the well-established alternatives first. Bore is not
battle tested, nor does it have a community behind it.

### About
Bore is a command-line utility for generating build files for Make and Ninja from build
templates written in Lua. That is, Bore does not build your project. It generates the inputs
to other build systems - primarily Make and Ninja - that _can_ build your project. It attempts
to address some of pain points of writing raw build files without having resorting to using
more complex build systems such as CMake, Meson, Gradle, etc.

The goal of Bore is to be as intuitive and fast as possible. Bore has a much smaller surface
area than most alternative meta build systems, and can be learned by looking at just a
handful of examples.

### Features
The list of features is pretty small, because the API is pretty small:
 * Automatic directory creation for build artifacts
 * Out-of-source build configuration
 * File globbing

## Getting Started

### Installation
There are currently no pre-built Bore binaries, so it must be built from scratch. It's only
dependency is `lib-lua`. The repository also has it's `build.ninja` file checked in so it
can be built without an existing bore executable. It's written in C, and can be compiled
pretty much anywhere if you already have Ninja and `lib-lua`.

```text
$ git clone git@github.com:ryanwebber/bore.git
$ cd bore
$ ninja
$ ninja install
```

### Running Bore
Bore can be run by simply running `bore --[ninja|make]`. The generator must be specified.
Bore looks for a `bore.lua` file in the same directory by default. The project directory
can be changed with the `-C` flag, and the default build file can be specified with the
`-f` flag, similar to both Make and Ninja. The project directory should nearly always be
set to the location of your source files, since the project directory is used as the
relative path during build file generation (see the [`env`](#envroot_dir) global).

You can also run Bore with `--dry-run` to parse and validate the build file without actually
generating any outputs.

### Generators
Bore supports 3 types of generators:
 * Make
 * Ninja
 * Dot

The Dot generator is used to create a graph describing the relationships between the inputs
and outputs of the targets described. 

#### Caveats
Because of subtle differences in the target build systems, there are minor caveats with
each.

| __Ninja__ | Ninja supports only one command, so targets that have multiple commands listed will be joined together. For example, two commands `a` and `b` will become `a && b`. |
| __Make__  | Makefiles don't support multiple outputs for a build rule. Targets that produce multiple outputs will be generated into a build rule for each output, with the same commands. This might cause non-optimal build rules in some cases, so in general multiple targets producing one output are preferred over fewer targets producing multiple outputs. |

## Build Templates
Bore build templates are written in Lua and you have access to the full suite of types
and libraries provided by the Lua Runtime. In addition, Bore includes a minimal API surface
in the Lua runtime that is used to define the targets of the project.

### Defining Targets
The main thing that build templates do is define the build targets of the project. In fact,
the only way the template can influence the generators is by defining targets. Targets
are defined by calling the global [`target(args)`](#targetargs) function. A target describes
how a [rule](#defining-rules) will be generated to the resulting build files.

#### Example
```lua
-- bore.lua
target {
    name = "MyTarget",
    build = rule {
        ins = { "input.txt" },
        outs = { "output.txt" },
        cmds = { "cp input.txt output.txt" }
    }
}
```

__Note:__ When calling a function with a string or table literal in Lua like we're doing
here, we can omit the parenthesis to the function.

This example creates a target named `"MyTarget"`, and will use the assiciated rule to determine
how to generate appropriate riles in the generated build files. Besides the `name` and `rule`
property of the table, there are several more that are described in the
[API Reference](#targetargs). 

The rule is a special object that defines the inputs, outputs, and commands for building
the target. We'll look more at rules in the next section.

The name of the target serves multiple purposes:
 1. Bore will use the name to generate a phony build rule for the target
 2. When a target is defined, Bore will place a special reference to it in the global
    [`targets`](#targets) table under it's defined name. This means you can reference the
    target when created other targets, which is particularly useful for specifying that the
    inputs to one target is the outputs of another. To continue our example above:
    ```lua
    target {
        name = "MyOtherTarget",
        build = rule {
            ins = targets.MyTarget.outputs,
            outs = { },
            cmds = { "echo ${ins}" }
        }
    }
    ```

    This second target will use the outputs of the first target as it's inputs, and will
    generate a build rule that will contain `echo output.txt`. You might also notice the
    special `${ins}` reference here, and it just means to interpolate the `ins` to the rule
    in place, but we'll also learn more about that in the next section.

Bore also has support for generating phony targets. A phony target is one that is
explicitly marked with `phony = true` in the target definition, or one that defines no
outputs. Targets that are explicitly marked as phony must define no outputs. Bore will
also attempt to automatically generate a phony build rule for each non-phony target
using the target name if there are no collisions with any concrete output files in the
project. 

The Makefile in our example above includes an automatic phony target and looks something
like:
```make
output.txt: input.txt
	cp input.txt output.txt

MyTarget: output.txt

.PHONY: MyTarget
```

With this generated Makefile, you could build `output.txt` by running `make output.txt` or
`make MyTarget`, which is a phony rule for the concrete `output.txt` build rule.

### Defining Rules
One of the main properties of targets is the build property. The build property must be a 
rule, which is a special type of object in Bore that is specially validated and interpolated,
but has no other real use. A rule can only be created by calling the global
[`rule(args)`](#ruleargs) function.

A rule is simply a set of input files, output files, required directories, and commands
used to create the output files from the input files. Most of these will
be familiar from Makefiles and Ninja build files except the directories property. Bore
will use the set of directories as _order-only_ prerequisites to the generated build
rules - making the directory the first time the outputs are build but not on subsequent
builds.

The `rule` function will also specially interpret and replace indexes of `${ins}` and 
`${outs}` in the `cmds` strings with a space-separated list of the inputs and outputs
to the rule respectively. Additionally, you can use `${<target_name>.ins}` and 
`${<target_name>.outs}` to interpolate the inputs and outputs from `<target_name>`
respectively.

For more information, see the [API Reference](#ruleargs) on the `rule` function.

### Environment
Each Bore template that gets evaluated has a special global variable called `env`. The
`env` variable contains information local to the current buld template being evaluated.

#### Example
```lua
-- bore.lua

-- Prints the project root directory
print(env.root_dir) -- ""

-- Prints the build directory configured with --build-dir
-- (defaults to "build/")
print(env.build_dir -- "build/"

-- Prints the local directory the current build template is in.
-- For the root build template, this is the same as the root_dir
print(env.local_dir)

-- The same as path.join(env.local_dir, "a", "b")
print(env.path("a", "b"))

-- The same as glob(path.join(env.local_dir, "*.txt"))
print(glob("*.txt"))

```

The root build templates (in fact, any build template) can be loaded by calling
[`include(path)`](#includepath) which will immediately load and evaluate the template.

## API Reference 

### `config`
The `config` table is populated with the key-value pairs passed to Bore via the `--config` flag. These can be used to configure the install prefix, details of the architecture, or other details that change how the build targets are generated.

#### Example
```lua
-- bore.lua

-- if bore was run with '--config message "Hello World!"', this
-- will print "Hello World!"
print(config.message)
```

### `env.build_dir`
The specified out-of-source build directory for the project. Configured with `--build-dir`.

### `env.local_dir`
The directory of the current build template being evaluated.

### `env.root_dir`
The root directory of the project. Specifically, this is equal to the `env.local_dir` of
the top-level build template.

### `rules`
A table containing the system build rules. See [the list of build rules](#) for more details.
The values of this table are also available in the global scope under the key name.

### `targets`
Targets that are defined can be accessed through the global `targets` table.
This is useful for chaining the inputs of one target to the output of another.

#### Example

```lua
target {
	name = "first",
	build = rule {
		outs = "output.txt"
		cmds = "echo 'Hello World!' > ${outs}"
	}
}

target {
	name = "second",
	phony = true,
	build = rule {
		ins = targets.first.outs, -- "output.txt"
		cmds = "cat ${ins}"
	}
}
```

Values in the `targets` table will have the following properties:

 * `outs` - The target outputs
 * `ins` - The target inputs
 * `dirs` - The target dirs
 * `cmds` - The target cmds
 * `name` - The target name
 * `phony` - If the target is a phony target
 * `default` - If the target is a default target

### `rule(args)`
Create a new rule primative. Again, we typically omit the brackets. Calling `rule` returns a special type of lua object that is expected when creating targets. The table passed to `rule` describe the inputs, output files, output directories, and commands for the target. 

| Parameter | Type | Description |
| --- | --- | --- |
| `ins` | `string`/`table` | A list of strings that represent the inputs to the target. |
| `outs` | `string`/`table` | A list of strings that represent the outputs of the target. |
| `dirs` | `string`/`table` | A list of strings that represent the directories required to be created before building the target can succeed. This typically is not explicitly set, and defaults to the directories of the `outs`. |
| `cmds` | `string`/`table` | A list of strings that represent the commands to run to build the `outs` from the `ins`. Command strings are expanded with the provided `ins` and `outs` by the special tokens `${ins}` and `${outs}` respectively. |

### `target(args)`
Define a new target. Lua syntax permits the developer to omit the brackets when calling a function with a table or string, so typically we omit them. The table passed to `target` describes how the target gets translated as build rules in the generated build files.

| Parameter | Type | Description |
| --- | --- | --- |
| `name` | `string` | The name of the target. Must be unique. __Required.__ |
| `phony` | `boolean` | Whether the targt should be considered phony or not. Phony targets must not describe any outputs. |
| `default` | `boolean` | Whether to include the target as part of the default build (ex. the `all` target in a Makefile, and as a default target in Ninja). |
| `build` | `rule` | A rule that describes how to build this target. __Required.__ |

