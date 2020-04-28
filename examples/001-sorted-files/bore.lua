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
		outs = module.path("outs/reversed.txt"),
		cmds = "sort ${ins} > ${outs}"
	}
}

