#ifndef KCC_STRING_H
#define KCC_STRING_H

#include <_builtin.h>

#if !defined(__KCC_JIT__)
#define strlen(size)                __kcc_builtin_strlen(size)
#else
int strlen(const char *s);
#endif
#define strcpy(dst, src)            __kcc_builtin_strcpy(dst, src)
#define strncpy(dst, src, len)      __kcc_builtin_strncpy(dst, src, len)
#define memset(dst, value, size)    __kcc_builtin_memset(dst, value, size)
#define memcpy(dst, src, size)      __kcc_builtin_memcpy(dst, src, size)
#define memmove(dst, src, size)     __kcc_builtin_memmove(dst, src, size)

char* strcat(char* s1, const char* s2);
char* strncat(char* s1, const char* s2, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char *s1, const char *s2, size_t n);
char* strchr(const char* s, int c);
size_t strcspn(const char *s1, const char *s2);
char* index(const char* s, int c);
char* strrchr(const char* s, int c);
char* rindex(const char* s, int c);
int memcmp(const void *s1, const void *s2, size_t n);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/string.c>
#elif defined(__KCC__)
#pragma import("string");
#endif
#endif

#endif  /* KCC_STRING_H */
