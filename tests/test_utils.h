#ifndef TEST_UTILS_H
#define TEST_UTILS_H

extern int tests_run;
extern int tests_passed;

#define CHECK(label, condition)                                                                    \
    do                                                                                             \
    {                                                                                              \
        tests_run++;                                                                               \
        if (condition)                                                                             \
        {                                                                                          \
            tests_passed++;                                                                        \
            printf("  PASS  %s\n", label);                                                         \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            printf("  FAIL  %s\n", label);                                                         \
        }                                                                                          \
    } while (0)

void print_summary(void);

#endif
