#ifndef PATH_H
#define PATH_H

#include <stdlib.h>

size_t path_join(const char *p1, const char *p2, char *buf, size_t len);
size_t path_join_many(const char **paths, char *buf, size_t len);
size_t path_dirname(const char *path, const char **index);
size_t path_basename(const char *path, const char **index);
size_t path_extension(const char *path, const char **index);
size_t path_filename(const char *path, const char **index);

#endif

