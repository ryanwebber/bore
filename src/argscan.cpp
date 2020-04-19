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

    defs[name].argc = 0;
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
        i++;
    }
}

