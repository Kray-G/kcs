#ifndef SYMBOL_H
#define SYMBOL_H
#if !defined(INTERNAL) || !defined(EXTERNAL)
# error Missing amalgamation macros
#endif

#include "string.h"
#include "token.h"
#include "type.h"

#include <stdio.h>

struct block;

enum symtype {
    SYM_DEFINITION = 0,
    SYM_TENTATIVE,
    SYM_DECLARATION,
    SYM_TYPEDEF,
    SYM_STRING_VALUE,
    SYM_CONSTANT,
    SYM_LABEL,
    SYM_TABLE,
    SYM_TABLE_ENTRY,
    SYM_TAG
};

/* Visibility of external declarations, LINK_NONE for other symbols. */
enum linkage {
    LINK_NONE = 0,
    LINK_INTERN,
    LINK_EXTERN
};

/*
 * A symbol represents declarations that may have a storage location at
 * runtime, such as functions, static and local variables.
 */
struct symbol {
    String name;
    Type type;

    uint32_t symtype : 8;
    uint32_t linkage : 8;
    uint32_t referenced : 1; /* Mark symbol as used. */
    uint32_t slot : 7;       /* Register allocation slot. */
    uint32_t index : 8;      /* Enumeration used in optimization. */

    /*
     * Tag to disambiguate temporaries, strings, constants, labels, and
     * scoped static variables.
     */
    int16_t n;

    /*
     * Scope depth; 0 for global, 1 for function parameters, > 1 for
     * local or scoped static variables.
     */
    int16_t depth;

    /*
     * Parameter or local variable offset to base pointer. This is kept
     * as 0 during parsing, but assigned when passed to back-end.
     *
     * Also used for index into .symtab during ELF generation.
     */
    int32_t stack_offset;

    /*
     * Global offset for global variable or static variable, -1 for local.
     */
    int32_t global_offset;

    union {
        /*
         * Hold a constant integral or floating point value. Used for
         * enumeration members and numbers which must be loaded from
         * memory in assembly code. Denoted by symtype SYM_CONSTANT.
         */
        union value constant;

        /*
         * String literals are also handled as symbols, having type []
         * const char. Denoted by symtype SYM_STRING_VALUE. Free string
         * constants are always named '.LC', disambiguated with n.
         */
        String string;

        /*
         * Symbols in label namespace hold a pointer to the block they
         * represent.
         */
        struct block *label;

        /*
         * Location in memory of variable length array. Set to current
         * position of stack pointer on IR_VLA_ALLOC, after subtracting
         * total size of the array.
         */
        const struct symbol *vla_address;
    } value;
};

/* Holds the declaration for memcpy, which is needed for codegen. */
INTERNAL  const struct symbol *decl_memcpy;

/* Get the full name, including numeric value to disambiguate. */
INTERNAL const char *sym_name(const struct symbol *sym);

/*
 * Determine if given symbol is a temporary value generated during
 * evaluation.
 */
INTERNAL int is_temporary(const struct symbol *sym);

/*
 * Create a floating point constant, which can be stored and loaded from
 * memory.
 */
INTERNAL struct symbol *sym_create_constant(Type type, union value val);

#endif
