#ifndef KCC_LIMITS_H
#define KCC_LIMITS_H

#include <_builtin.h>

#define CHAR_BIT        (8)
#define WORD_BIT        (32)
#define LONG_BIT        (64)

#define SCHAR_MIN       (-128)
#define SCHAR_MAX       (127)
#define UCHAR_MAX       (255)
#define CHAR_MIN        (-128)
#define CHAR_MAX        (127)
#define MB_LEN_MAX      (6)
#define SHRT_MIN        (-32768)
#define SHRT_MAX        (32767)
#define USHRT_MAX       (65535)
#define INT_MIN         (-2147483647 - 1)
#define INT_MAX         (2147483647)
#define UINT_MAX        (4294967295)
#define LONG_MIN        (-922337203685477580L - 1L)
#define LONG_MAX        (9223372036854775807L)
#define ULONG_MAX       (18446744073709551615UL)
#define LONGLONG_MIN    (-9223372036854775807LL - 1LL)
#define LONGLONG_MAX    (9223372036854775807LL)
#define ULONGLONG_MAX   (18446744073709551615ULL)

#ifndef PATH_MAX
#define PATH_MAX        (4096)
#endif

#endif  /* KCC_LIMITS_H */
