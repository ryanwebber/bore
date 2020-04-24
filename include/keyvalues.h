#ifndef KEYVALUES_H
#define KEYVALUES_H

struct KeyValueNode {
    char* key;
    char *value;
    struct KeyValueNode *next;
};

struct KeyValueList {
    struct KeyValueNode *first;
};

void kvp_free(struct KeyValueList *kvl);
void kvp_add(struct KeyValueList *kvl, const char* k, const char *v);
struct KeyValueNode* kvp_first(struct KeyValueList *kvl);
struct KeyValueNode* kvp_next(struct KeyValueNode *n);

#endif

