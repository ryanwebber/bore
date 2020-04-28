![CI](https://github.com/ryanwebber/bore/workflows/CI/badge.svg)
# Bore
A cross-platform build file generator for make and Ninja using templates written in plain lua.

Bore attempts to address some of pain points of writing raw build files (such as Makefiles and Ninja) without having to make the leap to learning complex build systems such as CMake, Meson, Gradle, etc. Writing build templates with bore should feel as simple and transparent as writing raw build files, but you get to do so in lua. You can learn pretty much everything about bore from just a couple of examples.

__Supported Generators:__

 * Makefile
 * Ninja
 * Dot (for dependency graphs)


__Features__

 * Easy out-of-source builds
 * Automatic directory creation dependencies
 * File pattern matching (ex. `*.txt`)


## Getting Started
Create a new build template:

```lua
-- bore.lua
target {
	name = "main",
	default = true,
	build = rule {
		cmds = "echo 'Hello World!'"
	}
}
```

Generate the build files and run a build:

```
$ bore --make
$ cat Makefile
all: hello

hello:
	echo Hello World!

.PHONY: all hello

$ make
echo Hello World!
Hello World!
```


## Installation
There are currently no pre-built binaries, so bore must be built from scratch. It has minimal dependencies. The repository also has it's `build.ninja` file checked in so it can be built without an existing bore executable.

```
$ git clone git@github.com:ryanwebber/bore.git
$ cd bore
$ ninja
$ ninja install
```


## Usage
Build templates describe the targets of the project that will be generated into build files by calling `target {...}`. This build template will then be evaluated, and the targets will be translated into the generated build files. Bore will attempt to load `bore.lua` in the current directory by default, and will generate build files in the current directory as well.

Build templates are ordinary lua files with a few extra variables and functions loaded into the global scope. Anything you can do with typical lua files can be done in build templates as well.


### Example
```lua

local outfile = module.path("outs/sorted.txt")

target {
	name = "bundle",
	build = rule {
		ins = module.glob("inputs/*.txt"),
		outs = module.object("bundle.txt"),
		cmds = "cat ${ins} > ${outs}"
	}
}

target {
	name = "sort",
	default = true,
	build = rule {
		ins = targets.bundle.outs,
		outs = outfile,
		cmds = "sort ${ins} > ${outs}"
	}
}
```

When the build file is generated from this template, and the default target is run (via `make all` or `ninja`), the following things will happen:
 
 1. The `build/` directory will be created (as the default out-of-source build directory)
 2. All `*.txt` files in `inputs/` will be concated into a `build/bundle.txt` as part of the `bundle` target
 3. The `outs/` directory will be created (required by the `sort` target)
 4. The lines in `build/bundle.txt` will be sorted and dumped into `outs/sorted.txt`


## API Reference 

### `target({ args })`
Define a new target. Lua syntax permits the developer to omit the brackets when calling a function with a table or string, so typically we omit them. The table passed to `target` describes how the target gets translated as build rules in the generated build files.

| Parameter | Type | Description |
| --- | --- | --- |
| `name` | `string` | The name of the target. Must be unique. __Required.__ |
| `phony` | `boolean` | Whether the targt should be considered phony or not. Phony targets must not describe any outputs. |
| `default` | `boolean` | Whether to include the target as part of the default build (ex. the `all` target in a Makefile, and as a default target in Ninja). |
| `build` | `rule` | A rule that describes how to build this target. __Required.__ |


### `rule({ args })`
Create a new rule primative. Again, we typically omit the brackets. Calling `rule` returns a special type of lua object that is expected when creating targets. The table passed to `rule` describe the inputs, output files, output directories, and commands for the target. 

| Parameter | Type | Description |
| --- | --- | --- |
| `ins` | `string`/`table` | A list of strings that represent the inputs to the target. |
| `outs` | `string`/`table` | A list of strings that represent the outputs of the target. |
| `dirs` | `string`/`table` | A list of strings that represent the directories required to be created before building the target can succeed. This typically is not explicitly set, and defaults to the directories of the `outs`. |
| `cmds` | `string`/`table` | A list of strings that represent the commands to run to build the `outs` from the `ins`. Command strings are expanded with the provided `ins` and `outs` by the special tokens `${ins}` and `${outs}` respectively. |


### `targets["<name>"]`
Once a target has been created, it can then be referenced later on via the global `targets` table. This is useful for chaining the inputs of one target to the output of another:

```lua
target {
	name = "first",
	build = rule {
		outs = "output.txt
		cmds = "echo 'Hello World!' > ${outs}"
	}
}

target {
	name = "second",
	phony = true,
	build = rule {
		ins = targets.first.outs,
		cmds = "cat ${ins}"
	}
}
```

Target objects in the `targets` table will have the following properties:

 * `outs` - The target outputs
 * `ins` - The target inputs
 * `dirs` - The target dirs
 * `cmds` - The target cmds
 * `name` - The target name
 * `phony` - If the target is a phony target
 * `default` - If the target is a default target

### `submodule("<path>")`
The top level `bore.lua` is the root build module. Modules can define submodules by calling `submodule(...)`. This function will immediately load and evaluate the submodule, and make any targets defined in it available through the `targets` global.

All modules have access to a global `module` object, that has several useful properties and functions. In fact, the only difference between modules is the state this object has.

```lua
-- bore.lua

submodule("other/build.lua")

local dir = module.local_dir
print(dir) -- ""
```

```lua
-- other/build.lua

local dir = module.local_dir
print(dir) -- "other"
```
