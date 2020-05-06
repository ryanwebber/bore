rules.c = {
    obj = function(args)
        local sources = optional.strings(args.sources, {})
        local flags = optional.strings(args.flags, {})
        local cc = optional.string(args.cc, "gcc")

        if #sources == 0 then
            fatal("Expected at least one c source")
        end

        local object = args.object
        if type(args.object) ~= "string" and #sources > 1 then
            fatal("Unable to infer destination object file")
        elseif type(args.object) ~= "string" then
            object = path.filename(sources[1]) .. ".o"
        end

        local build_dir = args.build_dir
        if type(args.build_dir) ~= "string" and #sources > 1 then
            fatal("Unable to infer build directory")
        elseif type(args.build_dir) ~= "string" then
            build_dir = path.dirname(sources[1])
        end

        return rule {
            ins = sources,
            outs = path.join(build_dir, object),
            cmds = string.format("%s %s -c -o ${outs} ${ins}", cc, table.concat(flags, " "))
        }
    end,
    executable = function(args)
        local objects = optional.strings(args.objects, {})
        local flags = optional.strings(args.flags, {})
        local libs = optional.strings(args.libs, {})
        local cc = optional.string(args.cc, "gcc")

        local binary = args.binary
        if type(binary) ~= "string" and #objects == 1 then
            binary = path.filename(binary)
        elseif type(binary) ~= "string" then
            fatal("Unable to infer binary output")
        end

        local bin_dir = args.bin_dir
        if type(args.bin_dir) ~= "string" and #sources > 1 then
            fatal("Unable to infer bin directory")
        elseif type(args.bin_dir) ~= "string" then
            bin_dir = path.dirname(objects[1])
        end

        binary = path.join(bin_dir, binary)

        return rule {
            ins = objects,
            outs = binary,
            cmds = string.format("%s %s ${ins} -o ${outs} %s",
                cc,
                table.concat(flags, " "),
                table.concat(libs, " "))
        }
    end
}

