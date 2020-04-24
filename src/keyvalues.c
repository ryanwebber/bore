#include <stdlib.h>

#include "keyvalues.h"
#include "utils.h"

void kvp_free(struct KeyValueList *kvl) {
    struct KeyValueNode *n = kvp_first(kvl);
    while (n != NULL) {
        struct KeyValueNode *next = kvp_next(n);
        free(n->key);
        free(n->value);
        free(n);

        n = next;
    }
}

void kvp_add(struct KeyValueList *kvl, const char *k, const char *v) {
    struct KeyValueNode *n = malloc(sizeof(struct KeyValueNode));
    n->key = strclone(k);
    n->value = strclone(v);
    n->next = kvl->first;
    kvl->first = n;
}

struct KeyValueNode* kvp_first(struct KeyValueList *kvl) {
    return kvl->first;
}

struct KeyValueNode* kvp_next(struct KeyValueNode *n) {
    return n->next;
}

