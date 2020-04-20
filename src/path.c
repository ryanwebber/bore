#include "path.h"
#include "cwalk.h"

size_t path_join(const char *p1, const char *p2, char *buf, size_t len) {
    return cwk_path_join(p1, p2, buf, len);
}

size_t path_join_many(const char **paths, char *buf, size_t len) {
    return cwk_path_join_multiple(paths, buf, len);
}

size_t path_dirname(const char *path, const char **index) {
    size_t len;
    cwk_path_get_dirname(path, &len);
    *index = path;
    return len;
}

size_t path_basename(const char *path, const char **index) {
    size_t len;
    cwk_path_get_basename(path, index, &len);
    return len;
}

size_t path_extension(const char *path, const char **index) {
    size_t len;
    const char *i;
    if (!cwk_path_get_extension(path, &i, &len))
        return 0;

    *index = i + 1;
    return len - 1;
}

size_t path_filename(const char *path, const char **index) {
    const char* ext_pos;
    if (path_extension(path, &ext_pos) == 0)
        return path_basename(path, index);

    if (path_basename(path, index) == 0)
        return 0;

    return ext_pos - (*index) - 1;
}

