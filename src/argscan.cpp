#include <sstream>
#include <iostream>

#include "argscan.h"

ArgScanner& ArgScanner::withOption(
        const char* name1,
        const char* name2,
        const int argc,
        const OptionCallback cb) {
    
    this->withOption(name1, argc, cb);
    this->withOption(name2, argc, cb);
    return *this;
}

ArgScanner& ArgScanner::withOption(
        const char* name,
        const int argc,
        const OptionCallback cb) {

    defs[name].argc = argc;
    defs[name].cb = cb;
    return *this;
}

void ArgScanner::scan(int argc, const char* argv[]) {
    int i = 1;
    while (i < argc) {
        const char* arg = argv[i];

        if (arg[i] != '-' || defs.find(arg) == defs.end()) {
            throw ArgScanException(std::string("Unexpected or unknown option '") + arg + "'");
        }

        auto def = defs.at(arg);
        if (i + def.argc >= argc) {
            std::stringstream ss;
            ss << "Option '" << arg << "' expects " << def.argc << " arguments but got " << (argc - i - 1);
            throw ArgScanException(ss.str());
        }

        std::cerr << arg << " " << def.argc << std::endl;

        i++;
    }
}

