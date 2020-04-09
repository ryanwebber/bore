#ifndef TARGET_H
#define TARGET_H

#include <memory>
#include <string>
#include <vector>

#include "rule.h"

class Target {
    private:
        std::string name;
        std::vector<std::shared_ptr<Rule>> rules;

    public:
        Target(std::string name);
        ~Target() = default;

        std::string getName() const;
        void addRule(const std::shared_ptr<Rule> rule);
};

#endif
