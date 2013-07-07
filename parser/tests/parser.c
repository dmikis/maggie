#include <stdlib.h>

#include <check.h>

#include "parser.h"

START_TEST (test) {
    ck_assert_int_eq(1, 2);
} END_TEST

Suite * parser_suite_create(void) {
    Suite * suite = suite_create("Parser");

    TCase * tc = tcase_create("core");
    tcase_add_test(tc, test);

    suite_add_tcase(suite, tc);

    return suite;
}
