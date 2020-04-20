#ifndef STRSET
#define STRSET

#include "list.h"
#include "tstree.h"

struct StringSet {
    struct List *values;
    struct TSTree *tree;
};

void sset_init(struct StringSet *ss);
void sset_free(struct StringSet *ss);

void sset_insert(struct StringSet *ss, const char *value);
bool sset_has(struct StringSet *ss, const char *value);

#endif

