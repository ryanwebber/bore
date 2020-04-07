#include <iostream>
#include <string>

#include "LuaRuntime.h"

int main(int argc, const char* argv[]) {

    if (argc < 2) {
        std::cout << "No files provided" << std::endl;
        return 1;
    }

    LuaRuntime runtime;
    runtime.load();
    runtime.evaluateScript("print(\"Hello world!\")");
    runtime.evaluateFile(argv[1]);
}

