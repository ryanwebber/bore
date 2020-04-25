#include "rule.h"
#include "fglob.h"

void rule_init(struct Rule *rule) {
    list_init(&rule->inputs);
    list_init(&rule->outputs);
    list_init(&rule->commands);
    list_init(&rule->dirs);
    list_init(&rule->deps);
}

void rule_free(struct Rule *rule) {
    list_free(&rule->inputs);
    list_free(&rule->outputs);
    list_free(&rule->commands);
    list_free(&rule->dirs);
    list_free(&rule->deps);
}

void rule_copy(struct Rule *dest, struct Rule *src) {
    list_copy(&dest->inputs, &src->inputs);
    list_copy(&dest->outputs, &src->outputs);
    list_copy(&dest->commands, &src->commands);
    list_copy(&dest->dirs, &src->dirs);
    list_copy(&dest->deps, &src->deps);
}

