#include <sstream>
#include "configuration_exception.h"

static std::string formatMessage(std::string &msg, const std::string &file) {
    std::stringstream ss;
    ss << msg << " (at '" << file << "')";
    return ss.str();
}

ConfigurationException::ConfigurationException(std::string msg):
    std::runtime_error(msg) {}

ConfigurationException::ConfigurationException(std::string msg, const std::string &file):
    std::runtime_error(formatMessage(msg, file)) {}

