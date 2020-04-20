#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>

struct Error {
    const char* msg;
};

void error_fmt(struct Error **err, const char* fmt, ...);

#endif

