#ifndef KCC_MATH_H
#define KCC_MATH_H

#include <_builtin.h>
#define __BIG_ENDIAN    0x01020304
#define __LITTLE_ENDIAN 0x04030201

#if defined(_WIN32) || defined(_WIN64)
#ifndef _HUGE_ENUF
#define _HUGE_ENUF  1e+300
#endif
#define INFINITY            ((float)(_HUGE_ENUF * _HUGE_ENUF))
#define HUGE_VAL            ((double)INFINITY)
#define HUGE_VALF           ((float)INFINITY)
#define HUGE_VALL           ((long double)INFINITY)
#define NAN                 ((float)(INFINITY * 0.0F))
#else
#ifndef __BYTE_ORDER
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
typedef union { unsigned char c[8]; double d; } _kcc_huge_val_t;
#if __BYTE_ORDER == __BIG_ENDIAN
#define _KCC_HUGE_VAL_BYTES	{ 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 }
#endif
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define _KCC_HUGE_VAL_BYTES	{ 0, 0, 0, 0, 0, 0, 0xf0, 0x7f }
#endif
#define HUGE_VAL	(((_kcc_huge_val_t){ _KCC_HUGE_VAL_BYTES }).d)
#endif

#define acos(x)             __kcc_builtin_math_acos(x)
#define asin(x)             __kcc_builtin_math_asin(x)
#define atan(x)             __kcc_builtin_math_atan(x)
#define atan2(y, x)         __kcc_builtin_math_atan2(y, x)
#define cos(x)              __kcc_builtin_math_cos(x)
#define sin(x)              __kcc_builtin_math_sin(x)
#define tan(x)              __kcc_builtin_math_tan(x)
#define cosh(x)             __kcc_builtin_math_cosh(x)
#define sinh(x)             __kcc_builtin_math_sinh(x)
#define tanh(x)             __kcc_builtin_math_tanh(x)
#define exp(x)              __kcc_builtin_math_exp(x)
#define frexp(value, exp)   __kcc_builtin_math_frexp(value, exp)
#define ldexp(x, exp)       __kcc_builtin_math_ldexp(x, exp)
#define log(x)              __kcc_builtin_math_log(x)
#define log10(x)            __kcc_builtin_math_log10(x)
#define modf(value, iptr)   __kcc_builtin_math_modf(value, iptr)
#define pow(x, y)           __kcc_builtin_math_pow(x, y)
#define sqrt(x)             __kcc_builtin_math_sqrt(x)
#define ceil(x)             __kcc_builtin_math_ceil(x)
#define fabs(x)             __kcc_builtin_math_fabs(x)
#define floor(x)            __kcc_builtin_math_floor(x)
#define fmod(x, y)          __kcc_builtin_math_fmod(x, y)

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#endif  /* KCC_MATH_H */
