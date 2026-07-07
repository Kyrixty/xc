#include <stdio.h>
#include "arena.h"
#define XC_IMPL
#include "xclua.h"
#include "xcfs.h"


int no_s_allowed(int c) {
    return c == 's';
}

static arena_t* globalArena;

int main(int argc, char** argv) {
    // BUG: hard-coded memory limit
    globalArena = arena_init(MB(64));

    if (argc != 2) {
        fprintf(stderr, "Usage: xc <path_to_lua_file>");
    }
    FILE* f = xc_fs_open(argv[1], "r");
    xc_lua_tokenize(globalArena, f);

    goto XC_CLEANUP;
XC_CLEANUP:
    xc_fs_close(f);
}