#ifndef TARGET_H
#define TARGET_H

#include "BuildNode.h"

class Target {
    private:
        std::string name;
        std::vector<BuildNode> nodes;
}

#endif

