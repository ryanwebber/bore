#ifndef PROJECTGRAPH_H
#define PROJECTGRAPH_H

#import "ProjectNode.h"

class ProjectGraph {
    private:
        std::map<std::string, std::shared_ptr<ProjectNode> nodes;

    public:
        ProjectGraph();
        ~ProjectGraph();
}

#endif

