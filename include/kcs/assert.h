#ifndef KCC_ASSERT_H
#define KCC_ASSERT_H

#undef assert

#ifdef NDEBUG

    #define assert(expression) ((void)0)

#else

    #include <stdio.h>
    #include <stdlib.h>
    void __kcc_assert(const char *message, const char *file, unsigned int line);
    #define assert(expression) (void)((!!(expression)) || (__kcc_assert(#expression, __FILE__, (unsigned)__LINE__), 0))

#endif

#endif  /* KCC_ASSERT_H */
