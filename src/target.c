#include <stdlib.h>
#include "target.h"

void target_free(struct Target *t) {
    rule_free(t->rule);
    free(t->rule);
    free(t->name);

    if (t->description != NULL)
        free(t->description);
}

