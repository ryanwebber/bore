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

    for (int i = 1; i < argc; i++) {
        int status = runtime.evaluateFile(argv[i]);
        if (status) {
            std::cout << "Whoops" << std::endl;
            break;
        }
    }
}

