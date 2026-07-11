#ifndef XC_ASSERTS_H
#define XC_ASSERTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static int tests_run;
static int tests_failed;

#define XC_EXIT_FAIL 247

#define LOG_FAIL(fmt, expected, actual) do { \
    printf("\033[1;31m[FAIL]\033[0m %s:%d\n", __FILE__, __LINE__); \
    printf("\tExpected: " fmt "\n", expected); \
    printf("\tActual:   " fmt "\n", actual); \
    tests_failed++; \
    exit(XC_EXIT_FAIL);    \
} while(0)

#define assert_true(expr) do { \
    tests_run++; \
    if (!(expr)) { \
        LOG_FAIL("%s", "true", #expr " evaluated to false"); \
    } \
} while(0)

#define assert_int_eq(expected, actual) do { \
    tests_run++; \
    int _exp = (expected); \
    int _act = (actual); \
    if (_exp != _act) { \
        LOG_FAIL("%d", _exp, _act); \
    } \
} while(0)

#define assert_str_eq(expected, actual) do { \
    tests_run++; \
    const char* _exp = (expected); \
    const char* _act = (actual); \
    if (strcmp(_exp, _act) != 0) { \
        LOG_FAIL("\"%s\"", _exp, _act); \
    } \
} while(0)

#define assert_float_eq(expected, actual, epsilon) do { \
    tests_run++; \
    double _exp = (expected); \
    double _act = (actual); \
    double _diff = _exp - _act; \
    if (_diff < 0) _diff = -_diff; \
    if (_diff > (epsilon)) { \
        LOG_FAIL("%f (w/ epsilon)", _exp, _act); \
    } \
} while(0)

#define assert_not_null(ptr) do { \
    tests_run++; \
    if ((ptr) == NULL) { \
        LOG_FAIL("%s", "non-NULL pointer", "NULL"); \
    } \
} while(0)

#endif // XC_ASSERT_H