#include <string>
#include <sstream>
#include "duplicate_module_exception.h"

static std::string formatException(
        std::shared_ptr<BuildModule> inserted,
        std::shared_ptr<BuildModule> existing) {
    std::stringstream ss;
    ss << "Module '" << inserted->getName()
        << "' defined in both "
        << inserted->getBuildFilePath()
        << " and "
        << existing->getBuildFilePath();

    return ss.str();
}

DuplicateModuleException::DuplicateModuleException(
        std::shared_ptr<BuildModule> inserted,
        std::shared_ptr<BuildModule> existing
        ): ConfigurationException(formatException(inserted, existing)) {
}

