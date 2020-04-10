#include "target.h"

Target::Target(std::string name, std::string module_name): name(name), module_name(module_name) {}

std::string Target::getName() const {
    return name;
}

std::string Target::getQualifiedName() const {
    return module_name + "." + name;
}

std::vector<std::shared_ptr<Rule>> Target::getRules() const {
    return rules;
}

void Target::addRule(const std::shared_ptr<Rule> rule) {
    rules.push_back(rule);
}

