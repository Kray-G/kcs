#ifndef CONTEXT_H
#define CONTEXT_H
#if !defined(INTERNAL) || !defined(EXTERNAL)
# error Missing amalgamation macros
#endif

#include <stddef.h>

enum target {
    TARGET_PREPROCESS,
    TARGET_IR_DOT,
    TARGET_IR_ASM,
    TARGET_IR_RUN,
    TARGET_IR_SAVE,
    TARGET_x86_64_ASM,
    TARGET_x86_64_JIT,
    TARGET_x86_64_JIT_ASM,
    TARGET_x86_64_OBJ,
    TARGET_x86_64_EXE,
};

enum cstd {
    STD_C89,
    STD_C99,
    STD_C11
};

/* Global information about translation unit. */
INTERNAL struct context {
    int32_t errors;
    int32_t verbose;
    int32_t suppress_warning;
    uint32_t pic : 1;            /* position independent code */
    uint32_t debug : 1;          /* Generate debug information. */
    enum target target;
    enum cstd standard;
} context;

/*
 * Output diagnostics info to stdout. No-op if context.verbose is not
 * set.
 */
INTERNAL void verbose(const char *, ...);

/*
 * Output warning to stderr. No-op if context.suppress_warning is set.
 */
INTERNAL void warning(const char *, ...);

/* Output error to stderr. */
INTERNAL void error(const char *, ...);

/* Control warning level. */
INTERNAL void disable_warning(const char *mode);

#endif
