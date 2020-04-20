#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "tstree.h"

static struct TSTree* tstree_ensure(struct TSTree **t, const char* key) {

    if (*t == NULL) {
        *t = malloc(sizeof(struct TSTree));
        tstree_init(*t);
        (*t)->c = key[0];
    }

    if ((*t)->c < key[0]) {
        return tstree_ensure(&((*t)->left), key);
    } else if ((*t)->c > key[0]) {
        return tstree_ensure(&((*t)->right), key);
    } else if ((*t)->c != '\0') {
        return tstree_ensure(&((*t)->children), &key[1]);
    } else {
        return *t;
    }
}

static struct TSTree* tstree_walk(struct TSTree *t, const char* key) {
    assert(t != NULL);

    if (t->c == key[0] && t->c == '\0') {
        return t->terminal ? t : NULL;
    } else if (t->c == key[0] && t->children != NULL) {
        return tstree_walk(t->children, &key[1]);
    } else if (t->c < key[0] && t->left != NULL) {
        return tstree_walk(t->left, key);
    } else if (t->c > key[0] && t->right != NULL) {
        return tstree_walk(t->right, key);
    } else {
        return NULL;
    }
}

void tstree_init(struct TSTree *t) {
    t->c = '\0';
    t->terminal = false;
    t->ud = NULL;
    t->children = NULL;
    t->left = NULL;
    t->right = NULL;
}

void tstree_free(struct TSTree *t) {
    if (t->children != NULL) {
        tstree_free(t->children);
        free(t->children);
    }

    if (t->left != NULL) {
        tstree_free(t->left);
        free(t->left);
    }

    if (t->right != NULL) {
        tstree_free(t->right);
        free(t->right);
    }
}

void* tstree_lookup(struct TSTree *t, const char *key) {
    struct TSTree *terminal = tstree_walk(t, key);
    if (terminal != NULL) {
        return terminal->ud;
    }

    return NULL;
}

bool tstree_test(struct TSTree *t, const char* key) {
    return tstree_walk(t, key) != NULL;
}


void* tstree_get_or_insert(struct TSTree *t, const char *key, void *ud) {
    assert(t != NULL);

    struct TSTree *t0 = tstree_ensure(&t, key);
    if (t0->terminal) {
        return t0->ud;
    }

    t0->terminal = true;
    t0->ud = ud;
    return NULL;
}

void tstree_insert(struct TSTree *t, const char *key, void *ud) {
    assert(t != NULL);

    struct TSTree *t0 = tstree_ensure(&t, key);
    t0->terminal = true;
    t0->ud = ud;
}

