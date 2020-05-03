![CI](https://github.com/ryanwebber/bore/workflows/CI/badge.svg)
# Bore
A cross-platform build file generator for make and Ninja using templates written in plain lua.

Bore attempts to address some of pain points of writing raw build files (such as Makefiles and Ninja) without having to make the leap to learning complex build systems such as CMake, Meson, Gradle, etc. Writing build templates with bore should feel as simple and transparent as writing raw build files, but you get to do so in lua. You can learn pretty much everything about bore from just a couple of examples. For more information, checkout the [documentation](https://ryanwebber.github.io/bore/).

__Note:__ Bore is a tool written to address my own needs. It's not battle tested, and probably contains some subtle bugs.

### Supported Generators

 * Makefile
 * Ninja
 * Dot (for dependency graphs)


### Features

 * Easy out-of-source builds
 * Automatic directory creation dependencies
 * Automatic help targets
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

local outfile = "outs/sorted.txt"

target {
	name = "bundle",
	build = rule {
		ins = glob("inputs/*.txt"),
		outs = "build/bundle.txt",
		cmds = "cat ${ins} > ${outs}"
	}
}

target {
	name = "sort",
	default = true,
	build = rule {
		ins = targets["bundle"].outs,
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

## Contributing
Please do! Feel free to open a PR for any and all issues.

## License
Bore is released under the [MIT License](https://opensource.org/licenses/MIT).
