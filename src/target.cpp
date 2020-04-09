#include "target.h"

Target::Target(std::string name): name(name) {}

std::string Target::getName() const {
    return name;
}

void Target::addRule(const std::shared_ptr<Rule> rule) {
    rules.push_back(rule);
}

