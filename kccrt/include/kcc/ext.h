#ifndef EXT_H
#define EXT_H

#include <_ext.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define KCC_CAPACITY_UNIT (256)

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - string
--------------------------------------------------------------------------------------------- */

#define KCC_CAPACITY_STR_UNIT (32)
#define KCC_CAPACITY(len) (((unsigned int)((len) / KCC_CAPACITY_STR_UNIT) + 1) * KCC_CAPACITY_STR_UNIT)

typedef struct string_ {
    char            *cstr;  // buffer of a string.
    unsigned int    cap;    // capacity of a buffer.
    unsigned int    len;    // actual length of a string.
} string_t;

extern string_t string_init_alloc(const char *cstr);
extern string_t string_substr(const string_t str, int start, int len);

#define string_reset(str, s) \
    if ((str)->cstr) { \
        string_clear(str); \
    } \
    if ((s) && ((s)[0] != 0)) { \
        string_append_cstr(str, s); \
    } \
    /**/

#define string_append_cstr_with_len_to_empty(lhs, rhs, rlen) \
    unsigned int cap = KCC_CAPACITY(rlen); \
    (lhs)->cstr = (char *)malloc(cap * sizeof(char)); \
    if (rlen > 0) { \
        memcpy((lhs)->cstr, rhs, rlen); \
    } \
    (lhs)->len = rlen; \
    (lhs)->cap = cap; \
    /**/

#define string_append_cstr_with_len_to_allocated(lhs, rhs, rlen) \
    int len = (lhs)->len + rlen; \
    if (len < (lhs)->cap) { \
        memcpy((lhs)->cstr + (lhs)->len, rhs, rlen); \
        (lhs)->cstr[len] = 0; \
        (lhs)->len = len; \
    } else { \
        unsigned int cap = KCC_CAPACITY(len); \
        char *buf = (char *)malloc(cap * sizeof(char)); \
        memcpy(buf, (lhs)->cstr, (lhs)->len); \
        memcpy(buf + (lhs)->len, rhs, rlen); \
        buf[len] = 0; \
        string_free(lhs); \
        (lhs)->len = len; \
        (lhs)->cap = cap; \
        (lhs)->cstr = buf; \
    } \
    /**/

#define string_append(lhs, rhs) \
    if (!(lhs)->cstr) { \
        string_append_cstr_with_len_to_empty(lhs, (rhs).cstr, (rhs).len); \
    } else { \
        string_append_cstr_with_len_to_allocated(lhs, (rhs).cstr, (rhs).len); \
    } \
    /**/

#define string_append_cstr_with_len(lhs, rhs, rlen) \
    if (!(lhs)->cstr) { \
        string_append_cstr_with_len_to_empty(lhs, rhs, rlen); \
    } else { \
        string_append_cstr_with_len_to_allocated(lhs, rhs, rlen); \
    } \
    /**/

#define string_append_cstr(lhs, rhs) \
    int rlen = strlen(rhs); \
    if (!(lhs)->cstr) { \
        string_append_cstr_with_len_to_empty(lhs, rhs, rlen); \
    } else { \
        string_append_cstr_with_len_to_allocated(lhs, rhs, rlen); \
    } \
    /**/

#define string_copy(str)    string_init(str.cstr);
#define string_free(str)    free((str)->cstr)
#define string_clear(str)   (((str)->cstr ? ((str)->cstr[0] = 0) : 0), (str)->len = 0)

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_string.c>
#endif
#endif

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - binary
--------------------------------------------------------------------------------------------- */

typedef struct binary_ {
    uint8_t      *buf;  // buffer of a byte.
    unsigned int cap;   // capacity of a buffer.
    unsigned int len;   // actual length of a binary.
} binary_t;

extern binary_t binary_init(const uint8_t *buf, int len);
extern binary_t binary_copy(const binary_t rhs);
extern void binary_append(binary_t* lhs, const binary_t rhs);
extern void binary_append_bytes(binary_t* lhs, const uint8_t *rhs, int len);
extern binary_t binary_subbin(const binary_t ary, int start, int len);

#define binary_free(bin)    free((bin)->buf)
#define binary_clear(bin)   ((bin)->len = 0)

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_binary.c>
#endif
#endif

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - vector
        This is from cvec: https://github.com/graphitemaster/cvec, which is Public Domain
--------------------------------------------------------------------------------------------- */

typedef struct {
    size_t allocated;
    size_t used;
} vector_t;

#define vector_of_(type, v) type* v = NULL

/* Attempts to grow [VECTOR] by [MORE]*/
#define vector_try_grow(VECTOR, MORE) \
    (((!(VECTOR) || vector_meta(VECTOR)->used + (MORE) >= vector_meta(VECTOR)->allocated)) ? \
        (void)vec_grow(((void **)&(VECTOR)), (MORE), sizeof(*(VECTOR))) : (void)0)

/* Get the metadata block for [VECTOR] */
#define vector_meta(VECTOR) \
    ((vector_t *)(((unsigned char *)(VECTOR)) - sizeof(vector_t)))

/* Deletes [VECTOR] and sets it to NULL */
#define vector_free(VECTOR) \
    ((void)((VECTOR) ? (vec_delete((void *)(VECTOR)), (VECTOR) = NULL) : 0))

/* Pushes back [VALUE] into [VECTOR] */
#define vector_push(VECTOR, VALUE) \
    (vector_try_grow((VECTOR), 1), (VECTOR)[vector_meta(VECTOR)->used++] = (VALUE))

/* Unshift front [VALUE] into [VECTOR] */
#define vector_unshift(VECTOR, VALUE) \
    (\
        vector_try_grow((VECTOR), 1),\
        memmove((VECTOR) + 1, (VECTOR), (vector_meta(VECTOR)->allocated - 1) * (sizeof(*(VECTOR)))),\
        (VECTOR)[0] = (VALUE),\
        ++(vector_meta(VECTOR)->used)\
    )

/* Get the size of [VECTOR] */
#define vector_size(VECTOR) \
    ((VECTOR) ? vector_meta(VECTOR)->used : 0)

/* Get the capacity of [VECTOR] */
#define vector_capacity(VECTOR) \
    ((VECTOR) ? vector_meta(VECTOR)->allocated : 0)

/* Resize [VECTOR] to accomodate [SIZE] more elements */
#define vector_resize(VECTOR, SIZE) \
    (vector_try_grow((VECTOR), (SIZE)), vector_meta(VECTOR)->used += (SIZE), \
        &(VECTOR)[vector_meta(VECTOR)->used - (SIZE)])

/* Get the head element in [VECTOR] */
#define vector_head(VECTOR) \
    ((VECTOR)[0])

/* Get the last element in [VECTOR] */
#define vector_last(VECTOR) \
    ((VECTOR)[vector_meta(VECTOR)->used - 1])

/* Pop an element off the back of [VECTOR] */
#define vector_pop(VECTOR) \
    ((void)(vector_meta(VECTOR)->used -= 1))

/* Shrink the size of [VECTOR] down to [SIZE] */
#define vector_shrinkto(VECTOR, SIZE) \
    ((void)(vector_meta(VECTOR)->used = (SIZE)))

/* Shrink [VECTOR] down by [AMOUNT] */
#define vector_shrinkby(VECTOR, AMOUNT) \
    ((void)(vector_meta(VECTOR)->used -= (AMOUNT)))

/* Append to [VECTOR], [COUNT] elements from [POINTER] */
#define vector_append(VECTOR, COUNT, POINTER) \
    ((void)(memcpy(vector_resize((VECTOR), (COUNT)), (POINTER), (COUNT) * sizeof(*(POINTER)))))

/* Remove from [VECTOR], [COUNT] elements starting from [INDEX] */
#define vector_remove(VECTOR, INDEX, COUNT) \
    ((void)(memmove((VECTOR) + (INDEX), (VECTOR) + (INDEX) + (COUNT), \
        sizeof(*(VECTOR)) * (vector_meta(VECTOR)->used - (INDEX) - (COUNT))), \
            vector_meta(VECTOR)->used -= (COUNT)))

void vec_grow(void **vector, size_t i, size_t s);
void vec_delete(void *vector);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_vector.c>
#endif
#endif

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - stack
--------------------------------------------------------------------------------------------- */

#define stack_of_(type, v)          vector_of_(type, v)
#define stack_free(STACK)           vector_free(STACK)
#define stack_push(STACK, VALUE)    vector_push(STACK, VALUE)
#define stack_size(STACK)           vector_size(STACK)
#define stack_capacity(STACK)       vector_capacity(STACK)
#define stack_last(STACK)           vector_last(STACK)
#define stack_pop(STACK)            ((void)(vector_meta(STACK)->used -= 1), ((STACK)[vector_meta(STACK)->used]))

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - queue
--------------------------------------------------------------------------------------------- */

#define queue_of_(type, v)          vector_of_(type, v)
#define queue_free(QUEUE)           vector_free(QUEUE)
#define queue_enqueue(QUEUE, VALUE) vector_unshift(QUEUE, VALUE)
#define queue_size(QUEUE)           vector_size(QUEUE)
#define queue_capacity(QUEUE)       vector_capacity(QUEUE)
#define queue_last(QUEUE)           vector_last(QUEUE)
#define queue_dequeue(QUEUE)        ((void)(vector_meta(QUEUE)->used -= 1), ((QUEUE)[vector_meta(QUEUE)->used]))

/* ---------------------------------------------------------------------------------------------
    KCC Extended Library - timer
--------------------------------------------------------------------------------------------- */

typedef void *timer_t;

timer_t timer_init(void);
double timer_elapsed(timer_t tmr);
void timer_free(timer_t tmr);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_timer.c>
#endif
#endif

#endif /* EXT_H */
