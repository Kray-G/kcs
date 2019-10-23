#ifndef KCC_STDLIB_RAND_C
#define KCC_STDLIB_RAND_C

#define KCC_NO_IMPORT
#include <stdlib.h>
#undef KCC_NO_IMPORT

static unsigned long __kcc_rand_seed = 1;

void srand(unsigned seed)
{
    __kcc_rand_seed = seed;
}

int rand(void)
{
    __kcc_rand_seed = __kcc_rand_seed * 1103515245L + 12345;
    return (unsigned)(__kcc_rand_seed / 65536L) % (RAND_MAX+1);
}

#endif
