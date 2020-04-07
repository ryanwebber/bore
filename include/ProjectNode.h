#ifndef PROJECTNODE_H
#define PROJECTNODE_H

class ProjectNode {
    private:
        std::map<std::string, std::shared_ptr<Target>> targets;

    public:
        ProjectNode();
        ~ProjectNode();
}

#endif

