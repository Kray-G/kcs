#ifndef KCC_CTYPE_H
#define KCC_CTYPE_H

#include <_builtin.h>

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

extern const unsigned char* __kcc_ctype;
#define __kcc_ctype_lookup(__c)     (__kcc_ctype[(int)(__c)])
#define isalpha(__c)            (__kcc_ctype_lookup(__c)&(_U|_L))
#define isupper(__c)            ((__kcc_ctype_lookup(__c)&(_U|_L))==_U)
#define islower(__c)            ((__kcc_ctype_lookup(__c)&(_U|_L))==_L)
#define isdigit(__c)            (__kcc_ctype_lookup(__c)&_D)
#define isxdigit(__c)           (__kcc_ctype_lookup(__c)&(_X|_D))
#define isspace(__c)            (__kcc_ctype_lookup(__c)&_S)
#define ispunct(__c)            (__kcc_ctype_lookup(__c)&_P)
#define isalnum(__c)            (__kcc_ctype_lookup(__c)&(_U|_L|_D))
#define isprint(__c)            (__kcc_ctype_lookup(__c)&(_P|_U|_L|_D|_B))
#define isgraph(__c)            (__kcc_ctype_lookup(__c)&(_P|_U|_L|_D))
#define iscntrl(__c)            (__kcc_ctype_lookup(__c)&_C)

int tolower(int c);
int toupper(int c);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/ctype.c>
#elif defined(__KCC__)
#pragma import("ctype");
#endif
#endif

#endif  /* KCC_CTYPE_H */
