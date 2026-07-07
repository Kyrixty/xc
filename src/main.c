#include <stdio.h>
#define XC_STRING_IMPL
#include "xcstring.h"
#include "arena.h"

int no_s_allowed(int c) {
    return c == 's';
}



int main() {
    XcStringView s = xcs(" this is a test  \n\t\t\t");
    s = xcs_trim(&s);
    while (!xcs_empty(&s)) {
        printf("|"XCS_FMT"|""\n", XCS_Arg(s));
        s = xcs_skip_until_after(&s, no_s_allowed);
    }
}