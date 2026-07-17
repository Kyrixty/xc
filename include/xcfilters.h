#ifndef XC_FILTERS_H
#define XC_FILTERS_H

#include <ctype.h>

// Helpful filter predicates for xcstring.

// [a-z], [A-Z], [0-9] or '.'
int xcf_is_alnum_or_dot(int c) { return isalnum(c) || c == '.'; }
int xcf_whitespace(int c) { return isspace(c); }
int xcf_not_whitespace(int c) { return !isspace(c); }

#endif // XC_FILTERS_H