#include "xcutils.h"
#include "xcassert.h"
#include "xccommon.h"

void test_memcpy() {
    char buf1[256];
    char buf2[256];
    for (size_t i = 0; i < 256; i++) {
        buf1[i] = i;
        buf2[i] = 256 - i - 1;
    }
    xc_memcpy(buf1, buf2, 256);
    for (size_t i = 0; i < 256; i++) {
        assert_true(buf1[i] == buf2[i]);
    }
}

void test_parse_digit() {
    for (int base = 2; base <= 36; base++) {
        for (int i = 0; i < base; i++) {
            char c1 = i >= 10 ? 'a' + (i - 10) : '0' + i;
            char c2 = i >= 10 ? 'A' + (i - 10) : '0' + i;
            // int x1 = xc_parse_digit(c1, base);
            // int x2 = xc_parse_digit(c2, base);
            // printf("[%d] %c %c, %d %d, %d\n", base, c1, c2, x1, x2, i);
            assert_true(xc_parse_digit(c1, base) == i);
            assert_true(xc_parse_digit(c2, base) == i);
        }
    }
}

void test_parse_int() {
    assert_true(xc_parse_int(NULL, 36, NULL) == 0);
    assert_true(xc_parse_int("0", 2, NULL) == 0);
    assert_true(xc_parse_int("00000", 2, NULL) == 0);
    assert_true(xc_parse_int("123", 10, NULL) == 123);
    assert_true(xc_parse_int("ff", 16, NULL) == 255);
    assert_true(xc_parse_int("zz", 36, NULL) == 35 * 36 + 35);
    assert_true(xc_parse_int("-123", 10, NULL) == -123);
    assert_true(xc_parse_int("     -123", 10, NULL) == -123);
}

int main() {
    test_memcpy();
    test_parse_digit();
    test_parse_int();
}