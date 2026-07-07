#ifndef XC_LUA_H
#define XC_LUA_H

#define XC_LUA_IMPL

#ifdef XC_LUA_IMPL
#define XC_LLEX_IMPL
#include "xclualex.h"
#include "arena.h"
#include <stdio.h>

XcLuaTokens xc_lua_tokenize(arena_t* mem, FILE* f) {
    // just a wrapper for now
    return xc_lualex_tokenize(mem, f);
}

#endif

#endif