#ifndef BUILDNODE_H
#define BUILDNODE_H

#include "BuildStep.h"

class BuildNode {
    private:
        std::shared_ptr<BuildStep> buildStep;
}

#endif

