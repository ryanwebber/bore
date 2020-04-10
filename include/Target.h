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
        std::vector<std::shared_ptr<Rule>> rules;

    public:
        Target(std::string name, std::string module_name);
        ~Target() = default;

        std::string getName() const;
        std::string getQualifiedName() const;
        std::vector<std::shared_ptr<Rule>> getRules() const;
        void addRule(const std::shared_ptr<Rule> rule);
};

#endif
