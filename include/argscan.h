#ifndef ARGSCAN_H
#define ARGSCAN_H

#include <map>
#include <string>
#include <functional>
#include <stdexcept>

typedef std::function<void(const char* [], int)> OptionCallback;

class ArgScanException: public std::runtime_error {
    public:
        ArgScanException(std::string msg):
            std::runtime_error(msg) {};
};

struct OptionDef {
    int argc;
    OptionCallback cb;
    /*
    OptionDef(): argc(0) {};
    OptionDef(int argc): argc(argc) {};
    OptionDef(OptionDef &other): argc {other.argc} {};
    OptionDef& operator=(const OptionDef &other) {
        argc = other.argc;
        return *this;
    };
    */
};

class ArgScanner {
    private:
        std::map<std::string, OptionDef> defs;

    public:
        ArgScanner() = default;
        ~ArgScanner() = default;

        ArgScanner& withOption(
                const char *name,
                const int argc,
                const OptionCallback cb);

        ArgScanner& withOption(
                const char *name1,
                const char *name2,
                const int argc,
                const OptionCallback cb);

        void scan(int argc, const char* argv[]);
};

#endif

