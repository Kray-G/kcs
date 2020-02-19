#ifndef KCC_ASSERT_ASSERT_C
#define KCC_ASSERT_ASSERT_C

#define KCC_NO_IMPORT
#include <stdio.h>
#undef KCC_NO_IMPORT

#pragma lib("stdio");

void __kcc_assert_fail(const char *file, int line, const char *msg)
{
    printf("Assertion failed at %s:%d\n", file, line);
    printf("Error: %s\n", msg);
    __kcc_builtin_abort();
}

#endif  /* KCC_ASSERT_ASSERT_C */
