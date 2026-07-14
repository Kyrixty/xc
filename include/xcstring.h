#ifndef XC_STRING_H
#define XC_STRING_H

#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "xccommon.h"

typedef struct {
    const char* __data;
    const char* data;
    size_t count;
} XcStringView;

void xcs_chop_left(XcStringView* xcs, size_t n);
void xcs_chop_right(XcStringView* xcs, size_t n);
void xcs_reset(XcStringView* xcs);
bool xcs_empty(const XcStringView* xcs);
bool xcs_startswith(const XcStringView* xcs, const XcStringView* prefix);
bool xcs_startswith_cstr(const XcStringView* xcs, const char* prefix);
/**
 * @return `int`   The index of the first occurrence of `target` found in `xcs`, or `-1` if no match was found.
 */
int xcs_index(const XcStringView* xcs, const XcStringView* target);
int xcs_index_cstr(const XcStringView* xcs, const char* target);
XcStringView xcs_trim_left(const XcStringView* xcs);
XcStringView xcs_trim_right(const XcStringView* xcs);
XcStringView xcs_trim(const XcStringView* xcs);
XcStringView xcs_collect(const XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_collect_until(const XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_skip(const XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_skip_until(const XcStringView* xcs, int(*predicate)(int c));
XcStringView xcs_split(const XcStringView* xcs, char c);
/**
 * Gets the character at index `i`.
 * Panics if `i` is outside of the indexable range of `xcs`,
 * that is, if `i >= xcs.count`
 */
char xcs_at(const XcStringView* xcs, size_t i);
/**
 * @return  `true`  if two `XcStringView`s are equal.
 */
bool xcs_str_eq(const XcStringView* lhs, const XcStringView* rhs);
/**
 * @return  `true`  if the data in `rhs` is equal to the data in `lhs`.
 */
bool xcs_eq_cstr(const XcStringView* lhs, const char* rhs);

#define XCS_FMT "%.*s"
#define XCS_Arg(xcs) (int)(xcs).count, (xcs).data

#define XC_STRING_IMPL
#ifdef XC_STRING_IMPL

#define XCS_PANIC_CODE 255
#define XCS_PANIC(FMT, ...) \
fprintf(stderr, "%s"FMT, "XCS_PANIC: ", __VA_ARGS__); \
exit(XCS_PANIC_CODE)

XcStringView xcs(const char* str) {
    return (XcStringView) {
        .__data = str,
        .data = str,
        .count = strlen(str),
    };
}

bool xcs_empty(const XcStringView* xcs) { 
    return xcs->count == 0 || xcs->__data == NULL;
}

void xcs_reset(XcStringView* xcs) {
    xcs->data = xcs->__data;
    xcs->count = strlen(xcs->__data);
}

inline char xcs_at(const XcStringView* xcs, size_t i) {
    if (i >= xcs->count) {
        XCS_PANIC("xcs_at: Index %llu is outside of indexable range. Have: 0-%llu.", i, xcs->count);
    }
    return xcs->data[i];
}

bool xcs_startswith(const XcStringView* xcs, const XcStringView* prefix) {
    if (xcs_empty(prefix)) {
        return true;
    }
    if (prefix->count > xcs->count) {
        return false;
    }
    for (size_t i = 0; i < prefix->count; i++) {
        if (prefix->data[i] != xcs->data[i]) {
            return false;
        }
    }
    return true;
}

bool xcs_startswith_cstr(const XcStringView* lhs, const char* prefix) {
    XcStringView s = xcs(prefix);
    return xcs_startswith(lhs, &s);
}

int xcs_index(const XcStringView* source, const XcStringView* target) {
    if (xcs_empty(target)) {
        return 0;
    }
    if (xcs_empty(source)) {
        return -1;
    }
    size_t left = 0, n = 0;
    for (; left < source->count; left += n) {
        bool matched = true;
        for (n = 0; n < target->count; n++) {
            if (xcs_at(source, left + n) != xcs_at(target, n)) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return left;
        }
        n = MAX(1, n); // if the first pass failed, then n = 0 and we will infinitely loop
    }
    return -1;
}

int xcs_index_cstr(const XcStringView* source, const char* target) {
    XcStringView _target = xcs(target);
    return xcs_index(source, &_target);
}

void xcs_chop_right(XcStringView* xcs, size_t n) {
    if (!xcs->count) return;
    xcs->count -= n;
}

void xcs_chop_left(XcStringView* xcs, size_t n) {
    if (n >= xcs->count) n = xcs->count;
    if (!xcs->count) return;
    xcs->data += n;
    xcs->count -= n;
}

XcStringView xcs_collect(const XcStringView* xcs, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && predicate(xcs->data[i])) {
        i++;
    }
    return (XcStringView) {
        .__data = xcs->__data,
        .data = xcs->data,
        .count = i,
    };
}

XcStringView xcs_collect_until(const XcStringView* xcs, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && !predicate(xcs->data[i])) {
        i++;
    }
    return (XcStringView) {
        .__data = xcs->__data,
        .data = xcs->data,
        .count = i,
    };
}

XcStringView xcs_skip(const XcStringView* xcs, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && predicate(xcs->data[i])) {
        i++;
    }
    XcStringView s = *xcs;
    if (i < xcs->count) {
        xcs_chop_left(&s, i);
    } else {
        xcs_chop_left(&s, s.count);
    }
    return s;

}

XcStringView xcs_skip_until(const XcStringView* xcs, int(*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && !predicate(xcs->data[i])) {
        i++;
    }
    if (i < xcs->count) {
        return (XcStringView) {
            .__data = xcs->__data,
            .data = xcs->data + i,
            .count = xcs->count - i,
        };
    }
    XcStringView s = *xcs;
    xcs_chop_left(&s, s.count);
    return s;
}

XcStringView xcs_trim_left(const XcStringView* xcs) {
    XcStringView s = xcs_skip(xcs, isspace);
    return s;
}

XcStringView xcs_trim_right(const XcStringView* xcs) {
    int i = xcs->count;
    size_t count = 0;
    XcStringView copy = *xcs;
    if (xcs->count == 0) return *xcs;
    while (i >= 0 && isspace(xcs->data[i - 1])) {
        i--;
        count++;
    }
    xcs_chop_right(&copy, count);
    return copy;
}

XcStringView xcs_trim(const XcStringView* xcs) {
    XcStringView copy = xcs_trim_right(xcs);
    return xcs_trim_left(&copy);
}



/* Is this bad? It definitely isn't thread-safe. */
static int __xcs_target = 0;
static inline int __xcs_split(int c) {
    return c == __xcs_target;
}

XcStringView xcs_split(const XcStringView* xcs, char c) {
    __xcs_target = c;
    return xcs_collect_until(xcs, __xcs_split);
}

bool xcs_str_eq(const XcStringView* lhs, const XcStringView* rhs) {
    if (!lhs && !rhs) { return true; }
    if (!lhs || !rhs) { return false; }
    if (!lhs->__data && !rhs->__data) { return true; }
    if (!lhs->__data || !rhs->__data) { return false; }
    if (lhs->count != rhs->count) { return false; }
    if (lhs->data == rhs->data && lhs->count == rhs->count) { return true; }
    for (size_t i = 0; i < lhs->count; i++) {
        if (lhs->data[i] != rhs->data[i])
            return false;
    }
    return true;
}

bool xcs_eq_cstr(const XcStringView* lhs, const char* rhs) {
    XcStringView __rhs = xcs(rhs);
    return xcs_str_eq(lhs, &__rhs);
}

#endif

#endif