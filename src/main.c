#include <stdio.h>
#define XC_STRING_IMPL
#include "xcstring.h"

int main() {
    XcStringView s = xcs("       \t\t\n  \n\t\t\t");
    s = xcs_trim(&s);
    printf("|"XCS_FMT"|""\n", XCS_Arg(s));
}