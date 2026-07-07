#ifndef XC_STRING_H
#define XC_STRING_H

#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    const char* __data;
    const char* data;
    size_t count;
} XcStringView;

void xcs_chop_left(XcStringView* xcs, size_t n);
void xcs_chop_right(XcStringView* xcs, size_t n);
void xcs_reset(XcStringView* xcs);
bool xcs_empty(XcStringView* xcs);
XcStringView xcs_trim_left(XcStringView* xcs);
XcStringView xcs_trim_right(XcStringView* xcs);
XcStringView xcs_trim(XcStringView* xcs);
XcStringView xcs_collect(XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_collect_until(XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_skip(XcStringView* xcs, int (*predicate)(int c));
XcStringView xcs_skip_until_after(XcStringView* xcs, int(*predicate)(int c));

#define XCS_FMT "%.*s"
#define XCS_Arg(xcs) (int)(xcs).count, (xcs).data

#define XC_STRING_IMPL
#ifdef XC_STRING_IMPL
bool xcs_empty(XcStringView* xcs) { 
    return xcs->count == 0;
}

void xcs_reset(XcStringView* xcs) {
    xcs->data = xcs->__data;
    xcs->count = strlen(xcs->__data);
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

XcStringView xcs_collect(XcStringView* xcs, int (*predicate)(int c)) {
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

XcStringView xcs_collect_until(XcStringView* xcs, int (*predicate)(int c)) {
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

XcStringView xcs_skip(XcStringView* xcs, int (*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && predicate(xcs->data[i])) {
        printf("%d\n", predicate(xcs->data[i]));
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

XcStringView xcs_skip_until_after(XcStringView* xcs, int(*predicate)(int c)) {
    size_t i = 0;
    while (i < xcs->count && !predicate(xcs->data[i])) {
        i++;
    }
    if (i < xcs->count) {
        return (XcStringView) {
            .__data = xcs->__data,
            .data = xcs->data + i + 1,
            .count = xcs->count - i - 1,
        };
    }
    XcStringView s = *xcs;
    xcs_chop_left(&s, s.count);
    return s;
}

XcStringView xcs_trim_left(XcStringView* xcs) {
    XcStringView s = xcs_skip(xcs, isspace);
    return s;
}

XcStringView xcs_trim_right(XcStringView* xcs) {
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

XcStringView xcs_trim(XcStringView* xcs) {
    XcStringView copy = xcs_trim_right(xcs);
    return xcs_trim_left(&copy);
}

XcStringView xcs(const char* str) {
    return (XcStringView) {
        .__data = str,
        .data = str,
        .count = strlen(str),
    };
}
#endif

#endif