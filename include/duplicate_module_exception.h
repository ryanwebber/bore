#ifndef DUPLICATE_MODULE_EXCEPTION_H
#define DUPLICATE_MODULE_EXCEPTION_H

#include <memory>
#include "build_module.h"
#include "configuration_exception.h"

class DuplicateModuleException: public ConfigurationException {
    public:
        DuplicateModuleException(
                std::shared_ptr<BuildModule> inserted,
                std::shared_ptr<BuildModule> existing);
};

#endif

