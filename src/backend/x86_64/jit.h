#ifndef JIT_H
#define JIT_H

#include "instr.h"

#include <stdio.h>

/* Call once on startup. */
INTERNAL void jit_init(void);

/*
 * Start processing symbol. If the symbol is static, data will follow.
 * If the symbol is of function type, instructions should follow. The
 * end of a symbol context is reached when this function is called
 * again, or on flush.
 */
INTERNAL int jit_symbol(const struct symbol *sym);

/* Add instruction to function context. */
INTERNAL int jit_text(struct instruction instr);

/* Add data to internal symbol context. */
INTERNAL int jit_data(struct immediate data);

/* Write any buffered data to output. */
INTERNAL int jit_run(void);
INTERNAL int jit_print(void);

/* Free memory after all objects have been compiled. */
INTERNAL int jit_finalize(void);

INTERNAL int jit_get_return_value(void);
INTERNAL void *jit_get_builtin_function(const char *label);
INTERNAL uint8_t jit_get_builtin_flbit(const char *label);
INTERNAL uint8_t jit_get_builtin_args(const char *label);

#endif
