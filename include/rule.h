#ifndef RULE_H
#define rule_H

#include <string>
#include <vector>

class Rule {
    private:
        std::vector<std::string> inputs;
        std::vector<std::string> outputs;
        std::vector<std::string> commands;
        std::vector<std::string> dirs;

    public:
        Rule() = default;
        Rule(Rule &other) = default;
        ~Rule() = default;

        void addInput(const std::string &input);
        void addOutput(const std::string &output);
        void addCommand(const std::string &command);
        void addDir(const std::string &dir);

        std::vector<std::string> getOutputs() const;
        std::vector<std::string> getInputs() const;
        std::vector<std::string> getCommands() const;
        std::vector<std::string> getDirs() const;
};

#endif

