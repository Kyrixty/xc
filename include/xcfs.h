#ifndef XC_FS_H
#define XC_FS_H
#include <stdio.h>

FILE* xc_fs_open(const char* fname, const char* mode);
void xc_fs_close(FILE* f);

#ifdef XC_IMPL
#define XC_FS_IMPL
#endif

#define XC_FS_IMPL

#ifdef XC_FS_IMPL

FILE* xc_fs_open(const char* fname, const char* mode) {
    FILE* f = fopen(fname, mode);
    return f;
}

void xc_fs_close(FILE* f) {
    fclose(f);
}

#endif
#endif