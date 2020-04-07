
local rule = function(args)
    print("Rule was called")
    return args
end

local M = {
    targets = {}
}

M.targets.main = rule {
    ins = {"input.txt"},
    outs = {"output.txt"},
    cmds = {
	    "cp ${ins} ${outs}"
    }
}

return M

