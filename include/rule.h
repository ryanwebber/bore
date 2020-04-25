#ifndef RULE_H
#define rule_H

#include "list.h"

struct Rule {
    struct List inputs;
    struct List outputs;
    struct List commands;
    struct List dirs;
    struct List deps;
};

void rule_init(struct Rule *rule);
void rule_free(struct Rule *rule);
void rule_copy(struct Rule *dest, struct Rule *src);

#endif

