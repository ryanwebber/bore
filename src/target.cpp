#include "target.h"

Target::Target(std::string name, std::shared_ptr<Rule>  rule): name(name), rule(rule) {}

std::string Target::getName() const {
    return name;
}

std::shared_ptr<Rule> Target::getRule() const {
    return rule;
}

