#include "test_utils.h"

#include <stdio.h>

int tests_run    = 0;
int tests_passed = 0;

/* ── helpers ─────────────────────────────────────────────── */

void print_summary(void)
{
    printf("\n%d / %d tests passed\n", tests_passed, tests_run);
}
