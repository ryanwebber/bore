#include <iostream>
#include <string>

#include "LuaRuntime.h"

int main() {
    LuaRuntime runtime;
    runtime.load();
    runtime.evaluateScript("print(\"Hello world!\")");
}

