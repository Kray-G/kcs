#ifndef STRING_H
#define STRING_H
#if !defined(INTERNAL) || !defined(EXTERNAL)
# error Missing amalgamation macros
#endif

#include <stddef.h>
#include <stdio.h>

#define SHORT_STRING_LEN 14

/*
 * Compact representation of strings, such as identifiers and literals.
 * Optimize for short lengths, storing strings of length < 14 inline in
 * the object itself. This type fits in 2 eightbytes.
 */
typedef union {
    struct {
        uint16_t len;
        char str[SHORT_STRING_LEN];
    } a;
    struct {
        uint16_t len;
        const char *str;
    } p;
    uint16_t len;
} String;

/* Inline construct a String object which fits in the small variant. */
#define SHORT_STRING_INIT(s) {{sizeof(s) - 1, s}}

/*
 * Get pointer to plain C string representation. This depends on the
 * type of string, whether it is short or long.
 */
#define str_raw(s) \
    ((s).len == 0 ? (const char *) "" \
        : (s).len < SHORT_STRING_LEN ? (const char *) (s).a.str : (s).p.str)

/* Free all allocated string area. */
INTERNAL void clear_string_all();

/* Initialize string, where the length can be determined by strlen. */
INTERNAL String str_init(const char *str);

/* Compare two strings, returning 0 if equal. */
INTERNAL int str_cmp(String s1, String s2);

/*
 * Output string to stream, in safe encoding for textual assembly or as
 * plain C code.
 */
INTERNAL int fprintstr(FILE *stream, String str);

/* Management allocation memory for string. */
INTERNAL const char *dup_string(const char *str);

#endif
