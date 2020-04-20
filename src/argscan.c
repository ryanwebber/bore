#include <stdlib.h>
#include <string.h>

#include "argscan.h"

static struct ArgHandler* argscan_lookup(struct ArgHandler *handles, const char* opt) {
    struct ArgHandler *h = handles;
    while (h->option != NULL && h->argc >= 0) {
        if (strcmp(h->option, opt) == 0) {
            return h;
        }

        h++;
    }

    return NULL;
}

void argscan(struct ArgHandler *handles, int argc, const char* argv[], struct Error **err) {
    int i = 1;
    while (i < argc) {
        const char* arg = argv[i];
        if (arg[0] != '-') {
            return error_fmt(err, "Unexpected argument '%s'", arg);
        }

        struct ArgHandler *h = argscan_lookup(handles, arg);
        if (h == NULL) {
            return error_fmt(err, "Unknown option '%s'", arg);
        }

        for (int a = 0; a < h->argc; a++) {
            if (i + a + 1 >= argc || argv[i + a + 1][0] == '-') {
                return error_fmt(err, "Option '%s' expects %d args but got %d", arg, h->argc, a);
            }
        }

        h->cb(&argv[i], h->argc);
        i += h->argc + 1;
    }
}

