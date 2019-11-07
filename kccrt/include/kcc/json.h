#ifndef JSON_H
#define JSON_H

#include <_ext.h>
#include <stdio.h>
#include <stdint.h>
#include <kcc/ext.h>

typedef enum {
    JSON_UNKNWON,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_PAIR,
    JSON_TEXT,
    JSON_BOOLEAN,
    JSON_INTEGER,
    JSON_REAL,
    JSON_NULL
} __json_type_t;

struct __json_object_;

typedef union __json_value_ {
    int                     b;          /* boolean      */
    int64_t                 i;          /* integer      */
    double                  d;          /* real         */
    string_t                t;          /* text         */
    struct __json_object_   *o;         /* object/array */
} __json_value_t;

typedef struct __json_object_ {
    __json_type_t           type;
    struct __json_object_   *link;      /* for object management */
    struct __json_object_   *root;      /* easy to access to the root */
    struct __json_object_   *mgr;       /* easy to access to the manager */

    struct __json_object_   *prop;      /* next element of object */
    struct __json_object_   *lobj;      /* last element of object */
    struct __json_object_   *next;      /* next element of array */
    struct __json_object_   *lary;      /* last element of array */

    string_t                key;        /* key if exists */
    __json_value_t          value;      /* value of json object */
} __json_object_t;

extern __json_object_t *__json_parse(const char *str);
extern __json_object_t *__json_parse_file(const char *filename);
extern const char *__json_error_message(void);

extern __json_object_t *__json_set_top(__json_object_t *j);
extern __json_object_t *__json_gen_object(void);
extern __json_object_t *__json_append_pair(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_gen_pair(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_gen_array(void);
extern __json_object_t *__json_append_value(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_gen_double_object(double d);
extern __json_object_t *__json_gen_integer_object(int64_t i);
extern __json_object_t *__json_gen_text_object(string_t s);
extern __json_object_t *__json_make_boolean(int);
extern __json_object_t *__json_make_null(void);

/* extension of calculation */
extern __json_object_t *__json_bit_or(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_bit_xor(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_bit_and(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_add(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_sub(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_mul(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_div(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_mod(__json_object_t *j1, __json_object_t *j2);
extern __json_object_t *__json_neg(__json_object_t *j);

typedef __json_object_t json_object_t;
#define json_yyin           __json_yyin
#define json_parse          __json_parse
#define json_parse_file     __json_parse_file
#define json_pretty_print   __json_pretty_print
#define json_free_all       __json_free_all
#define json_error_message  __json_error_message

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_json.c>
#endif
#endif

#endif /* JSON_H */
