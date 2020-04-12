#include "target.h"

Target::Target(std::string name, std::string module_name): name(name), module_name(module_name) {}

std::string Target::getName() const {
    return name;
}

std::string Target::getModuleName() const {
    return module_name;
}

std::string Target::getQualifiedName() const {
    return module_name + "." + name;
}

std::shared_ptr<Rule> Target::getRule() const {
    return rule;
}

void Target::setRule(const std::shared_ptr<Rule> rule) {
    this->rule = rule;
}

