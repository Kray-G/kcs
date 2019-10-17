#ifndef DLL_H
#define DLL_H

#include <stdint.h>

#define C_MAX_ARGS (10)
typedef enum arg_value_type_ {
    C_INT, C_UINT, C_DBL, C_STR, C_PTR
} arg_value_type_t;
typedef struct arg_type_ {
    arg_value_type_t type;
    union {
        int64_t  i;
        uint64_t u;
        double   d;
        char     *s;
        void     *p;
    } value;
} arg_type_t;

#endif /* DLL_H */
