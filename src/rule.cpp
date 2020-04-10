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

std::vector<std::string> Rule::getOutputs() const {
    return outputs;
}

std::vector<std::string> Rule::getInputs() const {
    return inputs;
}

std::vector<std::string> Rule::getCommands() const {
    return commands;
}

