#ifndef PROJECT_H
#define PROJECT_H

#import "ProjectGraph.h"

class Project {
    private:
        std::shared_ptr<ProjectGraph> graph;

    public:
        Project();
        ~Project();
}

#endif

