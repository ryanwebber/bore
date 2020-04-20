#include "fglob.h"

#ifndef _WIN32
    #include <glob.h>
#endif

static int fglob_err(const char *epath, int eerrno) {
    return 0;
}

int fglob(const char *pattern, struct List *l) {
#ifdef _WIN32
    list_add(l, pattern);
    return 1;
#else
    glob_t g;
    if (glob(pattern, GLOB_ERR, fglob_err, &g)) {
        return -1;
    }

    size_t count = g.gl_pathc;
    for (size_t i = 0; i < count; i++) {
        list_add(l, (const char*) g.gl_pathv[i]);
    }

    globfree(&g);

    return count;
#endif
}

