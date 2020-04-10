#ifndef generator
#define generator

#include "build_graph.h"
#include "build_module.h"

class Generator {
    public:
        virtual void generate(const BuildGraph &graph) = 0;
};

#endif

