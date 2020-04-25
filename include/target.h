#ifndef TARGET_H
#define TARGET_H

#include "rule.h"

struct Target {
    char *name;
    bool phony;
    struct Rule *rule;
};

void target_free(struct Target *t);

#endif

