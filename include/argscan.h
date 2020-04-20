#ifndef ARGSCAN_H
#define ARGSCAN_H

#include "error.h"

typedef void (*ArgCallback)(const char* argp[], int argc);

struct ArgHandler {
    const char *option;
    int argc;
    ArgCallback cb;
};

void argscan(struct ArgHandler *handles, int argc, const char* argv[], struct Error **err);

#endif

