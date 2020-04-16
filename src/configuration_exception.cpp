#include "configuration_exception.h"

ConfigurationException::ConfigurationException(std::string msg):
    std::runtime_error(msg) {}

