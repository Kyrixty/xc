#ifndef XC_UTILS_H
#define XC_UTILS_H
#include "xccommon.h"
#include <ctype.h>
#include <stdio.h>
#include <math.h>

void xc_memcpy(char* dst, const char* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        *dst++ = *src++;
    }
}

size_t xc_strlen(const char* s) {
    if (s == NULL) {
        return 0;
    }
    size_t i = 0;
    while (*s++ != 0) {
        i++;
    }
    return i;
}

/**
 * Parses a digit in a given base.
 * 
 * @param   c   the digit to parse
 * @param   base    the base to parse the digit in (min 2, max 36, will set to 2 if below, 36 if above.)
 * @return  the parsed digit value, or -1 on error.
 */
int xc_parse_digit(char c, int base) {
    base = MIN(MAX(base, 2), 36);
    int digit = -1;
    if ('0' <= c && c <= '9') {
        digit = c - '0';
    } else if ('a' <= c && c <= 'z') {
        digit = c - 'a' + 10;
    } else if ('A' <= c && c <= 'Z') {
        digit = c - 'A' + 10;
    }
    if (digit >= base || digit == -1) {
        return -1;
    }
    return digit;
}

/**
 * Parses an integer from a string.
 * 
 * @param       buf     the string to parse.
 * @param       base    the base to parse the string in.
 * @param[out]  nParsed outputs the number of characters parsed. Set this to NULL to ignore.
 * You can tell the parse was successful if `len(buf) == nParsed`.
 * @return the parsed integer
 * 
 * ```c
 * xc_parse_int(NULL, any_base);    // 0
 * xc_parse_int("0", 1);            // 0
 * xc_parse_int("123", 10)          // 123
 * xc_parse_int("-123", 10)         // -123
 * xc_parse_int("ff", 16);          // 255
 * ```
 */
int xc_parse_int(const char* buf, int base, size_t* nParsed) {
    size_t len = xc_strlen(buf);
    size_t i = 0;
    base = MIN(base, 36);
    if (base <= 1) {
        if (nParsed) {
            *nParsed = 0;
        }
        return 0;
    }
    while (i < len && isspace(buf[i])) {
        i++;
    }
    if (len - i == 0) {
        return 0;
    }
    int sign = 1;
    if (buf[i] == '-') {
        sign = -1;
        i++;
    } else if (buf[i] == '+') {
        i++;
    }

    int num = 0;
    while (i < len) {
        int digit = xc_parse_digit(buf[i], base);
        if (digit == -1) {
            break;
        }
        num *= base;
        num += digit;
        i++;
    }

    if (nParsed) {
        *nParsed = i;
    }
    return sign * num;
}

#endif // XC_UTILS_H