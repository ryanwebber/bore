#ifndef CONFIGURATION_EXCEPTION_H
#define CONFIGURATION_EXCEPTION_H

#include <stdexcept>

class ConfigurationException: public std::runtime_error {
    public:
        ConfigurationException(std::string msg);
        ConfigurationException(std::string msg, const std::string &file);
};

#endif
