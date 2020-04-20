#ifndef TARGET_H
#define TARGET_H

#include "rule.h"

struct Target {
    char *name;
    struct Rule *rule;
};

void target_free(struct Target *t);

#endif

