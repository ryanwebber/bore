#ifndef ARGOPTS_H
#define ARGOPTS_H

#include <string>

#include "vendor/argparse.hpp"

class ArgOpts {
    private:
        argparse::ArgumentParser &parser;

    public:
        ArgOpts(argparse::ArgumentParser &parser): parser(parser) {}
        ~ArgOpts() = default;

        template <typename T = std::string> T get(const char* name) const {
            return parser.get<T>(name);
        }
};

#endif

