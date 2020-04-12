#ifndef TARGET_H
#define TARGET_H

#include <memory>
#include <string>
#include <vector>

#include "rule.h"

class Target {
    private:
        std::string name;
        std::shared_ptr<Rule> rule;

    public:
        Target(std::string name, std::shared_ptr<Rule> rule);
        ~Target() = default;

        std::string getName() const;
        std::shared_ptr<Rule> getRule() const;
};

#endif

