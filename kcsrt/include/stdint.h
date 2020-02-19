#ifndef KCC_STDINT_H
#define KCC_STDINT_H

#include <_builtin.h>

/* Integer Types */
typedef char                int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

/* Limits of Specified-Width Integer Types */
#define INT8_MIN            ((int8_t)0x80)
#define INT8_MAX            (0x7f)
#define UINT8_MAX           (0xff)
#define INT16_MIN           ((int16_t)0x8000)
#define INT16_MAX           (0x7fff)
#define UINT16_MAX          (0xffff)
#define INT32_MIN           ((int32_t)0x80000000L)
#define INT32_MAX           (0x7fffffffL)
#define UINT32_MAX          (0xffffffffUL)
#define INT64_MIN           (-9223372036854775808LL)
#define INT64_MAX           (9223372036854775807LL)
#define UINT64_MAX          (18446744073709551615ULL)

typedef uint32_t            uintptr_t;
typedef int32_t             intptr_t;

#endif  /* KCC_STDINT_H */
