#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "utils.h"

void list_init(struct List* l) {
    l->first = NULL;
}

void list_free(struct List* l) {
    struct ListNode *node = list_first(l);
    while (node != NULL) {
        struct ListNode *next = list_next(node);
        free(node->value);
        free(node);

        node = next;
    }
}

bool list_empty(struct List* l) {
    return l->first == NULL;
}

void list_copy(struct List* dest, struct List* src) {
    struct ListNode* n = list_first(src);
    while (n != NULL) {
        list_add(dest, n->value);
        n = list_next(n);
    }
}

struct ListNode* list_add(struct List* l, const char* value) {
    struct ListNode *n = malloc(sizeof(struct ListNode));
    n->value = strclone(value);
    n->next = l->first;
    l->first = n;

    return n;
}

struct ListNode* list_first(struct List* l) {
    return l->first;
}

struct ListNode* list_next(struct ListNode* n) {
    return n->next;
}

