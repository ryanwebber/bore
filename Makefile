all: bore

bin/bore: build/utils.o build/tstree.o build/target.o build/strset.o build/rule.o build/path.o build/make_generator.o build/main.o build/lua_runtime.o build/lua_path.o build/lua_fglob.o build/list.o build/keyvalues.o build/graph_generator.o build/fglob.o build/error.o build/cwalk.o build/build_graph.o build/argscan.o build/__lua_embed.o | bin/
	gcc build/utils.o build/tstree.o build/target.o build/strset.o build/rule.o build/path.o build/make_generator.o build/main.o build/lua_runtime.o build/lua_path.o build/lua_fglob.o build/list.o build/keyvalues.o build/graph_generator.o build/fglob.o build/error.o build/cwalk.o build/build_graph.o build/argscan.o build/__lua_embed.o -o bin/bore -llua -lm -ldl

build/__lua_embed.o: build/bundle.lua | build/
	ld -r -b binary -o build/__lua_embed.o build/bundle.lua

build/bundle.lua: pkg/core.lua pkg/rules/phony.lua | build/
	cat pkg/core.lua pkg/rules/phony.lua > build/bundle.lua

build/argscan.o: src/argscan.c | build/
	gcc -g -Wall -I include -c -o build/argscan.o src/argscan.c

build/build_graph.o: src/build_graph.c | build/
	gcc -g -Wall -I include -c -o build/build_graph.o src/build_graph.c

build/cwalk.o: src/cwalk.c | build/
	gcc -g -Wall -I include -c -o build/cwalk.o src/cwalk.c

build/error.o: src/error.c | build/
	gcc -g -Wall -I include -c -o build/error.o src/error.c

build/fglob.o: src/fglob.c | build/
	gcc -g -Wall -I include -c -o build/fglob.o src/fglob.c

build/graph_generator.o: src/graph_generator.c | build/
	gcc -g -Wall -I include -c -o build/graph_generator.o src/graph_generator.c

build/keyvalues.o: src/keyvalues.c | build/
	gcc -g -Wall -I include -c -o build/keyvalues.o src/keyvalues.c

build/list.o: src/list.c | build/
	gcc -g -Wall -I include -c -o build/list.o src/list.c

build/lua_fglob.o: src/lua_fglob.c | build/
	gcc -g -Wall -I include -c -o build/lua_fglob.o src/lua_fglob.c

build/lua_path.o: src/lua_path.c | build/
	gcc -g -Wall -I include -c -o build/lua_path.o src/lua_path.c

build/lua_runtime.o: src/lua_runtime.c | build/
	gcc -g -Wall -I include -c -o build/lua_runtime.o src/lua_runtime.c

build/main.o: src/main.c | build/
	gcc -g -Wall -I include -c -o build/main.o src/main.c

build/make_generator.o: src/make_generator.c | build/
	gcc -g -Wall -I include -c -o build/make_generator.o src/make_generator.c

build/path.o: src/path.c | build/
	gcc -g -Wall -I include -c -o build/path.o src/path.c

build/rule.o: src/rule.c | build/
	gcc -g -Wall -I include -c -o build/rule.o src/rule.c

build/strset.o: src/strset.c | build/
	gcc -g -Wall -I include -c -o build/strset.o src/strset.c

build/target.o: src/target.c | build/
	gcc -g -Wall -I include -c -o build/target.o src/target.c

build/tstree.o: src/tstree.c | build/
	gcc -g -Wall -I include -c -o build/tstree.o src/tstree.c

build/utils.o: src/utils.c | build/
	gcc -g -Wall -I include -c -o build/utils.o src/utils.c

test: commontest sanitytest

sanitytest:
	test/sanity/maketest -r . -t /tmp/bore/maketest

commontest: bore
	cd test/common && lua runner.lua test-*

clean:
	rm -r bin
	rm -r build

install: bore
	install -d /usr/local/bin
	install -C  /usr/local/bin

bore: bin/bore

build/:
	@mkdir -p $@

bin/:
	@mkdir -p $@

.PHONY: all install clean commontest sanitytest test

