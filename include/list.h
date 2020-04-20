#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

struct ListNode {
    char *value;
    struct ListNode *next;
};

struct List {
    struct ListNode* first;
};

void list_free(struct List* l);
void list_init(struct List* l);

bool list_empty(struct List* l);
void list_copy(struct List* dest, struct List* src);

struct ListNode* list_add(struct List* l, const char *value);
struct ListNode* list_first(struct List* l);
struct ListNode* list_next(struct ListNode *n);


#endif

