#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"

#define ERR_BUF_LEN 512

static char err_buf[ERR_BUF_LEN];

void error_fmt(struct Error **err, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(err_buf, fmt, args);
    va_end(args);

    struct Error *e = malloc(sizeof(struct Error));
    e->msg = err_buf;
    *err = e;
}

