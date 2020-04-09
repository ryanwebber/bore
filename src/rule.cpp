#include "rule.h"

void Rule::addInput(const std::string &input) {
    inputs.push_back(input);
}

void Rule::addOutput(const std::string &output) {
    outputs.push_back(output);
}

void Rule::addCommand(const std::string &command) {
    commands.push_back(command);
}
