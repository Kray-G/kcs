#ifndef KCC_STRING_STRING_C
#define KCC_STRING_STRING_C

#include <stdlib.h>
#define KCC_NO_IMPORT
#include <string.h>
#undef KCC_NO_IMPORT

/* strcpy/strncpy is already difined as a built-in. */

char* strcat(char* s1, const char* s2)
{
    char* ss;
    for (ss = s1; *ss != '\0'; ++ss) {
        ;
    }
    for ( ; (*ss = *s2) != '\0'; ++ss, ++s2) {
        ;
    }
    return s1;
}

char* strncat(char* s1, const char* s2, size_t n)
{
    char* ss;
    char* end;
    for (ss = s1; *ss != '\0'; ss++) {
        ;
    }
    for (end = ss + n; (ss != end) && ((*ss = *s2) != '\0'); ++ss, ++s2) {
        ;
    }
    if (ss == end) {
        *ss = '\0';
    }
    return s1;
}

char* strdup(const char* s)
{
    char *p = malloc(strlen(s) * sizeof(char) + 1);
    strcpy(p, s);
    return p;
}

int strcmp(const char* s1, const char* s2)
{
    const unsigned char* ss1 = (const unsigned char*)s1;
    const unsigned char* ss2 = (const unsigned char*)s2;
    for ( ; *ss1 == *ss2 && *ss1 != '\0'; ++ss1, ++ss2) {
        ;
    }
    return *ss1 - *ss2;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    const unsigned char* ss1 = (const unsigned char*)s1;
    const unsigned char* ss2 = (const unsigned char*)s2;
    const unsigned char* t = ss1 + n;
    int r = 0;
    for ( ; ss1 != t && *ss1 != '\0' && ((r = *ss1 - *ss2) == 0); ++ss1, ++ss2) {
        ;
    }
    return r;
}

char* strchr(const char* s, int c)
{
    c = (char)c;
    do {
        if (*s == c)
            return (char*)s;
    }
    while (*s++ != '\0');
    return NULL;
}

size_t strcspn(const char *s1, const char *s2)
{
    const char *p = s1;
    for ( ; *s1; ++s1) {
        for (const char *t = s2; *t; t++) {
            if (*t == *s1) {
                return s1 - p;
            }
        }
    }
    return s1 - p;
}

char* index(const char* s, int c)
{
    return strchr(s, c);
}

char* strrchr(const char* s, int c)
{
    char* result = NULL;
    c = (char)c;
    do {
        if (*s == c)
            result = (char*)s;
    }
    while (*s++ != '\0');
    return result;
}

char* rindex(const char* s, int c)
{
    return strrchr(s, c);
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char* ss1 = (const unsigned char*)s1;
    const unsigned char* ss2 = (const unsigned char*)s2;
    const unsigned char* t = ss2 + n;
    int result = 0;
    for ( ; ss2 != t && (result = *ss1 - *ss2) == 0; ++ss1, ++ss2) {
        ;
    }
    return result;
}

#if !defined(__KCC_JIT__)
void *memcpy(void *s1, const void *s2, size_t n)
{
    char *ss1 = (char*)s1;
    const char *ss2 = (const char*)s2;

    if (n != 0) {
        const char *t = ss2 + n;
        do { *ss1++ = *ss2++; }
        while (ss2 != t);
    }
    return s1;
}
#endif

#endif  /* KCC_STRING_STRING_C */
