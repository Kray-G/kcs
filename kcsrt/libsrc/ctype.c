#ifndef KCC_CTYPE_CTYPE_C
#define KCC_CTYPE_CTYPE_C

#define KCC_NO_IMPORT
#include <ctype.h>
#undef KCC_NO_IMPORT

#ifndef __DEFINED_CTYPE_FLAGS
#define __DEFINED_CTYPE_FLAGS
#define _U  (0x01)
#define _L  (0x02)
#define _D  (0x04)
#define _S  (0x08)
#define _P  (0x10)
#define _C  (0x20)
#define _X  (0x40)
#define _B  (0x80)
#endif

#define __KCC_CTYPE_DATA_000_127 \
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C, \
    _C,     _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C,     _C, \
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C, \
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C, \
    _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P, \
    _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P, \
    _D,     _D,     _D,     _D,     _D,     _D,     _D,     _D, \
    _D,     _D,     _P,     _P,     _P,     _P,     _P,     _P, \
    _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U, \
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U, \
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U, \
    _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P, \
    _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L, \
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L, \
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L, \
    _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C

unsigned char __kcc_ctype_b[1 + 256] = {
    0,
    __KCC_CTYPE_DATA_000_127,
    /* all zero in remaining area. */
};
const unsigned char* __kcc_ctype = __kcc_ctype_b + 1;

int tolower(int c)
{
    /* only ascii is supported. */
    return isupper(c) ? (c) - 'A' + 'a' : c;
}

int toupper(int c)
{
    /* only ascii is supported. */
    return islower(c) ? c - 'a' + 'A' : c;
}

#endif  /* KCC_CTYPE_CTYPE_C */
