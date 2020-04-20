#include <stdlib.h>

#include "utils.h"

char* strclone(const char* str) {
    size_t len = 0;
    while (str[len++] != '\0') {}

    char *dup = (char*) calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; i++) {
        dup[i] = str[i];
    }

    dup[len] = '\0';

    return dup;
}

