#ifndef generator
#define generator

#include "build_graph.h"
#include "argopts.h"

class Generator {
    public:
        virtual void generate(const BuildGraph &graph, ArgOpts &opts) = 0;
};

#endif

