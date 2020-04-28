---
layout: default
---

{% include badge.html %}
# Bore

A cross-platform build file generator for make and Ninja using templates written in plain lua.

 * [Introduction](#)
    * [About](#)
    * [Features](#)
    * [Installation](#)
 * [Getting Starget](#)
    * [Running Bore](#)
    * [Generator Notes](#)
 * [Build Templates](#)
    * [Targets](#)
    * [Rules](#)
    * [Submodules](#)
 * [Examples](#)
 * [API Reference](#)
    * [Variables](#)
        * [`config`](#)
        * [`module.build_dir`](#)
        * [`module.local_dir`](#)
        * [`rules`](#)
        * [`targets`](#)

    * [Functions](#)
        * [`fatal()`](#)
        * [`glob()`](#)
        * [`module.glob()`](#)
        * [`module.object()`](#)
        * [`module.path()`](#)
        * [`path.basename()`](#)
        * [`path.dirname()`](#)
        * [`path.extension()`](#)
        * [`path.filename()`](#)
        * [`path.join()`](#)
        * [`rule()`](#ruleargs)
        * [`submodule()`](#submodulepath)
        * [`target()`](#targetargs)


## API Reference 

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

