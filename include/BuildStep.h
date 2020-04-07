#ifndef BUILDSTEP_H
#define BUILDSTEP_H

class BuildStep {
    private:
        std::vector<std::string> ins;
        std::vector<std::string> outs;
        std::vector<std::string> commands;
}

#endif
