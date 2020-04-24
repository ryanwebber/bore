#include <string.h>

#include "lua_fglob.h"
#include "fglob.h"

static int lua_fglob(lua_State *L) {
    size_t plen;
    const char* pattern = luaL_checklstring(L, 1, &plen);

    struct List l = { NULL };
    if (fglob(pattern, &l) < 0) {
        size_t blen = plen + 128;
        char buf[blen];
        snprintf(buf, blen, "No files matched from glob '%s'", pattern);
        lua_pushstring(L, buf);
        lua_error(L);

        list_free(&l);

        return 0;
    }

    lua_newtable(L);
    int index = 0;
    struct ListNode *n = list_first(&l);
    while (n != NULL) {
        lua_pushstring(L, n->value);
        lua_seti(L, -2, index + 1);

        index++;
        n = list_next(n);
    }

    list_free(&l);

    return 1;
}

int luaopen_fglob(lua_State *L) {
    lua_pushcfunction(L, lua_fglob);
    lua_setglobal(L, "glob");
    return 1;
}

