#include <stdlib.h>
#include "strset.h"

void sset_init(struct StringSet *ss) {
    ss->values = malloc(sizeof(struct List));
    list_init(ss->values);

    ss->tree = malloc(sizeof(struct TSTree));
    tstree_init(ss->tree);
}

void sset_free(struct StringSet *ss) {
    list_free(ss->values);
    free(ss->values);

    tstree_free(ss->tree);
    free(ss->tree);
}

void sset_insert(struct StringSet *ss, const char *value) {
    if (sset_has(ss, value)) {
        return;
    }

    tstree_insert(ss->tree, value, NULL);
    list_add(ss->values, value);
}

bool sset_has(struct StringSet *ss, const char *value) {
    return tstree_test(ss->tree, value);
}

