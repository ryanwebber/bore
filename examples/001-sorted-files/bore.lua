target {
	name = "bundle",
	build = rule {
		ins = env.glob("inputs/*.txt"),
		outs = env.object("bundle.txt"),
		cmds = "cat ${ins} > ${outs}"
	}
}

target {
	name = "sort",
	default = true,
	build = rule {
		ins = targets.bundle.outs,
		outs = env.path("outs/reversed.txt"),
		cmds = "sort ${ins} > ${outs}"
	}
}

