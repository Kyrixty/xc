#ifndef XC_STRING_H
#define XC_STRING_H

#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "xccommon.h"
#include "xcerror.h"
#include "xcutils.h"

typedef struct {
    const char* __data;
    const char* data;
    size_t __count;
    size_t count;
} XcStringView;

void xcs_chop_left(XcStringView* xcs, size_t n);
void xcs_chop_right(XcStringView* xcs, size_t n);
void xcs_reset(XcStringView* xcs);
/**
 * Clears `xcs`. Note that it will keep it's position,
 * but `xcs->count` will be set to 0.
 */
void xcs_clear(XcStringView* xcs);
/**
 * Advances the internal char* in an
 * `XcStringView` to the end, clearing it
 * at the same time via `xcs_clear`.
 */
void xcs_consume_all(XcStringView* xcs);
bool xcs_empty(const XcStringView* xcs);
bool xcs_startswith(const XcStringView* xcs, const XcStringView* prefix);
bool xcs_startswith_cstr(const XcStringView* xcs, const char* prefix);
bool xcs_endswith(const XcStringView* xcs, const XcStringView* prefix);
bool xcs_endswith_cstr(const XcStringView* xcs, const char* prefix);
/**
 * @return  `true`  if `xcs` contains `target`. `false` otherwise.
 */
bool xcs_has(const XcStringView* xcs, const XcStringView* target);
bool xcs_has_cstr(const XcStringView* xcs, const char* target);
#define XCS_NOT_FOUND -1
/**
 * @return `int`   The index of the first occurrence of `target` found in `xcs`, or `XCS_NOT_FOUND` if no match was found.
 */
int xcs_index(const XcStringView* s, const XcStringView* target);
int xcs_index_cstr(const XcStringView* s, const char* target);
XcStringView xcs_trim_left(const XcStringView* s);
XcStringView xcs_trim_right(const XcStringView* s);
XcStringView xcs_trim(const XcStringView* s);
/**
 * Collects part of a string view while `predicate` returns true.
 * Does not include the character which yielded `predicate` to
 * return false.
 * 
 * @param   `s` the string view to operate on
 * @param   `predicate` a function which will be called on each character.
 * If `predicate` returns `false`, this will terminate the returned string
 * immediately before the current character being processed.
 * @return  an `XcStringView` representing the part of the string
 * where `predicate` did not yet return false.
 * 
 * ```c
 * int collect_non_l(int c) {
 *      return c != 'l';
 * }
 * XcStringView s = xcs("hello");
 * XcStringView he = xcs_collect(&s, collect_non_l);
 * printf(XCS_FMT"\n", XCS_Arg(he)); // "he"
 * ```
 */
XcStringView xcs_collect(const XcStringView* s, int (*predicate)(int c));
/**
 * Collects part of a string view until `predicate` returns true.
 * Does not include the character which yielded `predicate` to
 * return true.
 * 
 * @param   `s` the string view to operate on
 * @param   `predicate` a function which will be called on each character.
 * If `predicate` returns `true`, this will terminate the returned string
 * immediately before the current character being processed.
 * @return  an `XcStringView` representing the part of the string
 * where `predicate` did not yet return true.
 * 
 * ```c
 * int collect_until_l(int c) {
 *      return c == 'l';
 * }
 * XcStringView s = xcs("hello");
 * XcStringView he = xcs_collect_until(&s, collect_until_l);
 * printf(XCS_FMT"\n", XCS_Arg(he)); // "he"
 * ```
 */
XcStringView xcs_collect_until(const XcStringView* s, int (*predicate)(int c));
XcStringView xcs_skip(const XcStringView* s, int (*predicate)(int c));
XcStringView xcs_skip_until(const XcStringView* s, int(*predicate)(int c));
/**
 * Returns the first substring of `s` which does not contain `c`.
 * Similar to `xcs_collect_until`, but operates on a direct char
 * instead of a given function predicate.
 * 
 * @param   `s` the string to be split
 * @param   `c` the character to split `s` by.
 * @return  The leading string before `c` is found.
 * 
 * ```c
 * XcStringView s = xcs("hello");
 * XcStringView he = xcs_split(&s, 'l'); // "he"
 * ```
 */
XcStringView xcs_split(const XcStringView* s, char c);
/**
 * Returns a string view representing the substring of `s`
 * from indexes `start` to `end` (including the char at `start`, but not at end.
 * Similar to python's `x[i:j]` slice.)
 * 
 * @return  XcStringView    the substring from `start` to `end`.
 */
XcStringView xcs_substring(const XcStringView* s, size_t start, size_t end);
/**
 * Gets the character at index `i`.
 * Panics if `i` is outside of the indexable range of `xcs`,
 * that is, if `i >= xcs.count`
 */
char xcs_at(const XcStringView* xcs, size_t i);
/**
 * @return  `true`  if two `XcStringView`s are equal.
 */
bool xcs_eq(const XcStringView* lhs, const XcStringView* rhs);
/**
 * @return  `true`  if the data in `rhs` is equal to the data in `lhs`.
 */
bool xcs_eq_cstr(const XcStringView* lhs, const char* rhs);

#define XCS_FMT "%.*s"
#define XCS_Arg(xcs) (int)(xcs).count, (xcs).data

#define XC_STRING_IMPL
#ifdef XC_STRING_IMPL


XcStringView xcs(const char* str) {
    return (XcStringView) {
        .__data = str,
        .data = str,
        .__count = xc_strlen(str),
        .count = xc_strlen(str),
    };
}

bool xcs_empty(const XcStringView* xcs) { 
    return xcs->count == 0 || xcs->__data == NULL;
}

void xcs_reset(XcStringView* xcs) {
    xcs->data = xcs->__data;
    xcs->count = xc_strlen(xcs->__data);
}

void xcs_clear(XcStringView* xcs) {
    xcs->count = 0;
}

void xcs_consume_all(XcStringView* xcs) {
    xcs->data = xcs->__data + xcs->__count;
    xcs_clear(xcs);
}

inline char xcs_at(const XcStringView* xcs, size_t i) {
    if (i >= xcs->count) {
        XCS_ERROR("xcs_at: Index %llu is outside of indexable range. Have: 0-%llu.", i, xcs->count == 0 ? 0 : xcs->count - 1);
    }
    return xcs->data[i];
}

int xcs_index(const XcStringView* source, const XcStringView* target) {
    if (xcs_empty(target)) {
        return 0;
    }
    if (xcs_empty(source) || target->count > source->count) {
        return XCS_NOT_FOUND;
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
    return XCS_NOT_FOUND;
}

int xcs_index_cstr(const XcStringView* source, const char* target) {
    XcStringView _target = xcs(target);
    return xcs_index(source, &_target);
}

int xcs_index_char(const XcStringView* source, char target) {
    char buf[2] = {target, 0};
    return xcs_index_cstr(source, buf);
}

bool xcs_startswith(const XcStringView* xcs, const XcStringView* prefix) {
    return xcs_index(xcs, prefix) == 0;
    // if (xcs_empty(prefix)) {
    //     return true;
    // }
    // if (prefix->count > xcs->count) {
    //     return false;
    // }
    // for (size_t i = 0; i < prefix->count; i++) {
    //     if (prefix->data[i] != xcs->data[i]) {
    //         return false;
    //     }
    // }
    // return true;
}

bool xcs_startswith_cstr(const XcStringView* lhs, const char* prefix) {
    XcStringView s = xcs(prefix);
    return xcs_startswith(lhs, &s);
}

bool xcs_endswith(const XcStringView* s, const XcStringView* suffix) {
    if (xcs_empty(suffix)) {
        return true;
    }
    if (xcs_empty(s)) {
        return false;
    }
    if (suffix->count > s->count) {
        return false;
    }
    for (size_t i = 0; i < suffix->count; i++) {
        if (xcs_at(s, s->count - suffix->count + i) != xcs_at(suffix, i)) {
            return false;
        }
    }
    return true;
}

bool xcs_endswith_cstr(const XcStringView* s, const char* suffix) {
    XcStringView target = xcs(suffix);
    return xcs_endswith(s, &target);
}

bool xcs_has(const XcStringView* xcs, const XcStringView* target) {
    return xcs_index(xcs, target) != XCS_NOT_FOUND;
}

bool xcs_has_cstr(const XcStringView* source, const char* target) {
    XcStringView _target = xcs(target);
    return xcs_has(source, &_target);
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

XcStringView xcs_collect(const XcStringView* s, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < s->count && predicate(xcs_at(s, i))) {
        i++;
    }
    return (XcStringView) {
        .__data = s->__data,
        .__count = s->__count,
        .data = s->data,
        .count = i,
    };
}

XcStringView xcs_collect_until(const XcStringView* s, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < s->count && !predicate(xcs_at(s, i))) {
        i++;
    }
    return (XcStringView) {
        .__data = s->__data,
        .__count = s->__count,
        .data = s->data,
        .count = i,
    };
}

XcStringView xcs_skip(const XcStringView* s, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < s->count && predicate(xcs_at(s, i))) {
        i++;
    }
    XcStringView cpy = *s;
    if (i < s->count) {
        xcs_chop_left(&cpy, i);
    } else {
        xcs_chop_left(&cpy, cpy.count);
    }
    return cpy;

}

XcStringView xcs_skip_until(const XcStringView* s, int(*predicate)(int c)) {
    size_t i = 0;
    while (i < s->count && !predicate(xcs_at(s, i))) {
        i++;
    }
    if (i < s->count) {
        return (XcStringView) {
            .__data = s->__data,
            .data = s->data + i,
            .count = s->count - i,
        };
    }
    XcStringView cpy = *s;
    xcs_chop_left(&cpy, cpy.count);
    return cpy;
}

XcStringView xcs_trim_left(const XcStringView* xcs) {
    XcStringView s = xcs_skip(xcs, isspace);
    return s;
}

XcStringView xcs_trim_right(const XcStringView* s) {
    int i = s->count;
    size_t count = 0;
    XcStringView copy = *s;
    if (s->count == 0) return *s;
    while (i >= 0 && isspace(xcs_at(s, i - 1))) {
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

XcStringView xcs_substring(const XcStringView* s, size_t start, size_t end) {
    if (!s->count) {
        XCS_ERROR("xcs_substring: called on empty string (start=%llu, end=%llu).", start, end);
    }
    if (end < start) {
        XCS_ERROR("xcs_substring: end < start (start=%llu, end=%llu)", start, end);
    }
    if (end > s->count) {
        XCS_ERROR("xcs_substring: end index out of bounds (start=%llu, end=%llu, but can only slice from [0:%llu])", start, end, s->count);
    }
    return (XcStringView) {
        .count = MAX((int)(end - start), 0),
        .data = s->data + start,
        .__data = s->__data,
        .__count = s->__count
    };
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

bool xcs_eq(const XcStringView* lhs, const XcStringView* rhs) {
    if (!lhs && !rhs) { return true; }
    if (!lhs || !rhs) { return false; }
    if (!lhs->__data && !rhs->__data) { return true; }
    if (!lhs->__data || !rhs->__data) { return lhs->count == rhs->count; } // empty XCS and NULL are equal
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
    return xcs_eq(lhs, &__rhs);
}

#endif

#endif