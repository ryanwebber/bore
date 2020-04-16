#include "generation_exception.h"

GenerationException::GenerationException(std::string msg):
    std::runtime_error(msg) {}

