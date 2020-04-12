#ifndef TARGET_H
#define TARGET_H

#include <memory>
#include <string>
#include <vector>

#include "rule.h"

class Target {
    private:
        std::string name;
        std::string module_name;
        std::shared_ptr<Rule> rule;

    public:
        Target(std::string name, std::string module_name);
        ~Target() = default;

        std::string getName() const;
        std::string getModuleName() const;
        std::string getQualifiedName() const;
        std::shared_ptr<Rule> getRule() const;
        void setRule(std::shared_ptr<Rule> rule);
};

#endif

