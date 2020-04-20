#ifndef TSTREE_H
#define TSTREE_H

struct TSTree {
    char c;
    bool terminal;
    void *ud;

    struct TSTree *children;
    struct TSTree *left;
    struct TSTree *right;
};

void tstree_init(struct TSTree *t);
void tstree_free(struct TSTree *t);

void* tstree_lookup(struct TSTree *t, const char *key);
bool tstree_test(struct TSTree *t, const char *key);

void tstree_insert(struct TSTree *t, const char *key, void *ud);
void* tstree_get_or_insert(struct TSTree *t, const char *key, void* ud);

#endif

