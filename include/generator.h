#ifndef generator
#define generator

#include "build_graph.h"

class Generator {
    public:
        virtual void generate(const BuildGraph &graph) = 0;
};

#endif

