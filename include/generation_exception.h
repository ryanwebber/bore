#ifndef GENERATION_EXCEPTION_H
#define GENERATION_EXCEPTION_H

#include <stdexcept>

class GenerationException: public std::runtime_error {
    public:
        GenerationException(std::string msg);
};

#endif

