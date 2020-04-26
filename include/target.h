#ifndef TARGET_H
#define TARGET_H

#include "rule.h"

struct Target {
    char *name;
    bool phony;
    bool primary;
    bool alias;
    struct Rule *rule;
};

void target_free(struct Target *t);

#endif

