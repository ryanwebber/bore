#include "fglob.h"

#ifndef _WIN32
    #include <glob.h>
#endif

static int glob_err(const char *epath, int eerrno) {
    return 0;
}

int fglob(const std::string &pattern, std::vector<std::string> &results) {
#ifdef _WIN32
    results.push_back(pattern);
    return 0;
#else
    glob_t g;
    if (glob(pattern.c_str(), GLOB_ERR, glob_err, &g)) {
        return 1;
    }

    for (size_t i = 0; i < g.gl_pathc; i++) {
        results.push_back(g.gl_pathv[i]);
    }

    globfree(&g);

    return 0;
#endif
}

