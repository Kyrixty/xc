#include "xcstring.h"
#include "xcassert.h"
#include <stdio.h>

int skip_l(int c) {
    return c == 'l';
}

int skip_until_o(int c) {
    return c == 'o';
}

int collect_l(int c) {
    return c == 'l';
}

int collect_until_o(int c) {
    return c == 'o';
}

void test_str_eq() {
    XcStringView s1 = xcs("hello");
    XcStringView s2 = xcs("hello");
    XcStringView s3 = xcs("bad");
    assert_true(xcs_eq(NULL, NULL));
    assert_true(xcs_eq(&s1, &s2));
    assert_true(!xcs_eq(&s1, &s3));
    assert_true(!xcs_eq(&s2, &s3));
    assert_true(xcs_at(&s1, 0) == 'h');

    assert_true(xcs_eq_cstr(&s1, strdup("hello")));

    assert_true(xcs_startswith(&s1, &s2));
    assert_true(xcs_startswith_cstr(&s1, "he"));
    assert_true(xcs_startswith_cstr(&s2, "he"));
    assert_true(!xcs_startswith(&s1, &s3));
    assert_true(!xcs_startswith(&s2, &s3));
    assert_true(!xcs_startswith_cstr(&s1, "s1 doesn't start with this!"));

    assert_true(xcs_index(&s1, &s2) == 0);
    assert_true(xcs_index_cstr(&s1, "e") == 1);
    assert_true(xcs_index_cstr(&s1, "a") == -1);

    xcs_chop_right(&s2, 1);
    assert_true(!xcs_eq(&s1, &s2));
    xcs_chop_right(&s2, s2.count);
    xcs_chop_right(&s1, s1.count);
    assert_true(xcs_eq(&s1, &s2));
    xcs_reset(&s1);
    xcs_reset(&s2);
    xcs_chop_left(&s1, 2);
    xcs_chop_left(&s2, 2);
    assert_true(xcs_eq(&s1, &s2));

    assert_true(xcs_index(&s1, &s2) == 0);
    assert_true(xcs_index_cstr(&s1, "l") == xcs_index_cstr(&s2, "l"));
    assert_true(xcs_index_cstr(&s1, "l") == 0);
    assert_true(xcs_startswith(&s1, &s2));
    assert_true(xcs_startswith_cstr(&s1, "llo"));
    assert_true(xcs_startswith_cstr(&s1, "ll"));
    assert_true(xcs_at(&s1, s1.count - 1) == 'o');
    assert_true(xcs_has(&s1, &s2));
    assert_true(xcs_has_cstr(&s1, "ll"));
    assert_true(xcs_has_cstr(&s2, "o"));
    assert_true(!xcs_has(&s1, &s3));
    assert_true(!xcs_has_cstr(&s1, "This isn't in s1!"));
    assert_true(!xcs_has_cstr(&s1, "llox"));
    s1 = xcs_skip(&s1, skip_l);
    s2 = xcs_skip_until(&s2, skip_until_o);
    assert_true(xcs_eq(&s1, &s2));
    xcs_reset(&s1);
    xcs_reset(&s2);
    assert_true(xcs_eq(&s1, &s2));
    xcs_chop_left(&s1, 2);
    xcs_chop_left(&s2, 2);
    s1 = xcs_collect(&s1, collect_l);
    s2 = xcs_collect_until(&s2, collect_until_o);
    assert_true(xcs_eq(&s1, &s2));

    xcs_clear(&s1);
    xcs_clear(&s2);
    xcs_consume_all(&s3);
    assert_true(xcs_eq(&s1, &s2));
    assert_true(xcs_eq(&s2, &s3));
    assert_true(xcs_eq(&s1, &s3));
    assert_true(xcs_eq_cstr(&s1, NULL));
}

int main() {
    test_str_eq();
}