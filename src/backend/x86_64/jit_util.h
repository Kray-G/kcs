#ifndef JIT_UTIL_H
#define JIT_UTIL_H

INTERNAL void jit_create(void **buffer, int size);
INTERNAL void jit_destroy(void *buffer, int size);
INTERNAL int jit_execute_int(void *main_pos);
INTERNAL uint64_t jit_execute_uint64(void *main_pos);
INTERNAL double jit_execute_double(void *main_pos);
INTERNAL void print_stack(void);

#define jit_execute(m) jit_execute_int(m)

#endif
