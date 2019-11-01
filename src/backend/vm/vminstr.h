#ifndef VMINSTR_H
#define VMINSTR_H

#include <lacc/context.h>
#include "../../parser/symtab.h"

enum vm_opcode {
    VM_NOP,
    VM_LABEL,
    VM_HALT,
    VM_STORE,
    VM_FZERO,
    VM_CALL,
    VM_RET,
    VM_CLUP,
    VM_ENTER,
    VM_DEREF,
    VM_CAST,
    VM_ALLOCA,
    VM_JMP,
    VM_JZ,
    VM_JNZ,
    VM_PUSH,
    VM_POP,
    VM_DEPOP,
    VM_NOT,
    VM_NEG,
    VM_ADD,
    VM_SUB,
    VM_MUL,
    VM_DIV,
    VM_MOD,
    VM_AND,
    VM_OR,
    VM_XOR,
    VM_SHL,
    VM_SHR,
    VM_EQ,
    VM_NE,
    VM_GE,
    VM_GT,
    VM_LE,
    VM_LT,

    VM_INC,
    VM_DEC,
    VM_JMPTBL,
    VM_TBL_ENTRY,

    VM_GLOBAL,
    VM_REFLIB,
    VM_SAVE_RETVAL,
};

#if defined(__GNUC__)
#define KCCVM_DEFINE_DISPATCH_TABLE()\
    static const void* vm_dispatch_table[] = {\
        &&LABEL_VM_NOP, \
        &&LABEL_VM_LABEL, \
        &&LABEL_VM_HALT, \
        &&LABEL_VM_STORE, \
        &&LABEL_VM_FZERO, \
        &&LABEL_VM_CALL, \
        &&LABEL_VM_RET, \
        &&LABEL_VM_CLUP, \
        &&LABEL_VM_ENTER, \
        &&LABEL_VM_DEREF, \
        &&LABEL_VM_CAST, \
        &&LABEL_VM_ALLOCA, \
        &&LABEL_VM_JMP, \
        &&LABEL_VM_JZ, \
        &&LABEL_VM_JNZ, \
        &&LABEL_VM_PUSH, \
        &&LABEL_VM_POP, \
        &&LABEL_VM_DEPOP, \
        &&LABEL_VM_NOT, \
        &&LABEL_VM_NEG, \
        &&LABEL_VM_ADD, \
        &&LABEL_VM_SUB, \
        &&LABEL_VM_MUL, \
        &&LABEL_VM_DIV, \
        &&LABEL_VM_MOD, \
        &&LABEL_VM_AND, \
        &&LABEL_VM_OR, \
        &&LABEL_VM_XOR, \
        &&LABEL_VM_SHL, \
        &&LABEL_VM_SHR, \
        &&LABEL_VM_EQ, \
        &&LABEL_VM_NE, \
        &&LABEL_VM_GE, \
        &&LABEL_VM_GT, \
        &&LABEL_VM_LE, \
        &&LABEL_VM_LT, \
        &&LABEL_VM_INC, \
        &&LABEL_VM_DEC, \
        &&LABEL_VM_JMPTBL, \
        &&LABEL_VM_HALT, \
        &&LABEL_VM_HALT, \
        &&LABEL_VM_SAVE_RETVAL, \
    };\
    /**/
#define VM_START()      struct vm_code *code = base[ip];\
                        goto *vm_dispatch_table[code->opcode]; {\
                        /**/
#define VM_END()        LABEL_VM_END_LOOP:;
#define VM_CASE_(op)    LABEL_ ## op
#define VM_CASE_DEFAULT LABEL_VM_DEFAULT
#define NEXT()          code = base[ip];\
                        goto *vm_dispatch_table[code->opcode];\
                        /**/
#define VM_GOTO_END()   goto LABEL_VM_END_LOOP;
#else // !defined(__GNUC__)
#define KCCVM_DEFINE_DISPATCH_TABLE()
#define VM_START()      struct vm_code *code;\
                        NEXT();\
                        /**/
#define VM_END()        LABEL_VM_END_LOOP:;
#define VM_CASE_(op)    LBL_ ## op
#define VM_CASE_DEFAULT if (0) { LABEL_VM_DEFAULT
#define VM_GOTO_END()   goto LABEL_VM_END_LOOP;
#define VM_GOTO_S()     code = base[ip]; switch (code->opcode) {
#define VM_GOTO_L(op)   case op: goto LBL_ ## op
#define VM_GOTO_E()     }
#define NEXT()          VM_GOTO_S()\
    VM_GOTO_L(VM_NOP); \
    VM_GOTO_L(VM_LABEL); \
    VM_GOTO_L(VM_HALT); \
    VM_GOTO_L(VM_STORE); \
    VM_GOTO_L(VM_FZERO); \
    VM_GOTO_L(VM_CALL); \
    VM_GOTO_L(VM_RET); \
    VM_GOTO_L(VM_CLUP); \
    VM_GOTO_L(VM_ENTER); \
    VM_GOTO_L(VM_DEREF); \
    VM_GOTO_L(VM_CAST); \
    VM_GOTO_L(VM_ALLOCA); \
    VM_GOTO_L(VM_JMP); \
    VM_GOTO_L(VM_JZ); \
    VM_GOTO_L(VM_JNZ); \
    VM_GOTO_L(VM_PUSH); \
    VM_GOTO_L(VM_POP); \
    VM_GOTO_L(VM_DEPOP); \
    VM_GOTO_L(VM_NOT); \
    VM_GOTO_L(VM_NEG); \
    VM_GOTO_L(VM_ADD); \
    VM_GOTO_L(VM_SUB); \
    VM_GOTO_L(VM_MUL); \
    VM_GOTO_L(VM_DIV); \
    VM_GOTO_L(VM_MOD); \
    VM_GOTO_L(VM_AND); \
    VM_GOTO_L(VM_OR); \
    VM_GOTO_L(VM_XOR); \
    VM_GOTO_L(VM_SHL); \
    VM_GOTO_L(VM_SHR); \
    VM_GOTO_L(VM_EQ); \
    VM_GOTO_L(VM_NE); \
    VM_GOTO_L(VM_GE); \
    VM_GOTO_L(VM_GT); \
    VM_GOTO_L(VM_LE); \
    VM_GOTO_L(VM_LT); \
    VM_GOTO_L(VM_INC); \
    VM_GOTO_L(VM_DEC); \
    VM_GOTO_L(VM_JMPTBL); \
    VM_GOTO_L(VM_SAVE_RETVAL); \
    VM_GOTO_E();\
    /**/

#endif // defined(__GNUC__)

enum vm_optype {
    VMOP_NONE,
    VMOP_LABEL,
    VMOP_INT8,
    VMOP_INT16,
    VMOP_INT32,
    VMOP_INT64,
    VMOP_UINT8,
    VMOP_UINT16,
    VMOP_UINT32,
    VMOP_UINT64,
    VMOP_FLT,
    VMOP_DBL,
    VMOP_LDBL,
    VMOP_CHARP,
    VMOP_VARI,
    VMOP_VARU,
    VMOP_VARFL,
    VMOP_VAROBJ,
    VMOP_ADDR,
    VMOP_FUNCNAME,
    VMOP_FUNCADDR,
    VMOP_BUILTIN,
};

struct vm_func_args_info {
    array_of(struct expression) expr;
    int size;
};

struct vm_label {
    int index;
    String name;
};

struct vm_cast {
    Type src;
    Type dst;
};

typedef int (*vm_builtin_t)(uint8_t *, int64_t);

struct vm_address {
    String name;
    int is_global;
    int index;
    int base;
    int offset;
    int size;
    vm_builtin_t func;
};

union vm_imm {
    uint8_t byte;
    uint16_t word;
    uint32_t dword;
    uint64_t qword;
    float f;
    double d;
    long double ld;
    char* str;
    struct {
        uint8_t v[16];
    } value;
};

struct vm_code {
    int index;
    enum vm_opcode opcode;
    enum vm_optype type;

    union {
        int lindex;
        int size;
        String name;
        struct vm_cast cast;
        struct vm_address addr;
        union vm_imm imm;
    } d;
};

#define VM_GLOBAL_MEM_SIZE (16*1024*1024)
struct vm_program {
    uint8_t *global;
    array_of(struct vm_code) code;
    array_of(struct vm_code*) exec;
};

struct vm_context {
    FILE *stream;
    const char *file;
    int global_index;
    int stack_offset;
    array_of(struct vm_label) globals;
    array_of(struct vm_label) labels;
    array_of(String) imports;
};

#define is_temporary_var(var)       (str_raw((var).symbol->name)[0] == '.')
#define is_string_var(var)          ((var).kind == IMMEDIATE && (var).symbol && (var).symbol->symtype == SYM_STRING_VALUE)
#define emit_vm_code(item)          array_push_back((is_global_mode ? &vm_glbl.code : &vm_prog.code), (item))
#define vm_code_len()               array_len(&vm_prog.code)

#define STACK_TOPI()                *(uint64_t*)(stack+sp)
#define STACK_TOPIT(type)           *(type*)(stack+sp)
#define STACK_TOPD()                *(double*)(stack+sp)
#define STACK_TOPDT(type)           *(type*)(stack+sp)
#define STACK_TOPLD()               *(long double*)(stack+sp)
#define STACK_TOPA()                (stack+sp)
#define STACK_TOPI_OFFSET(o)        *(uint64_t*)(stack+sp+(o))
#define STACK_TOPIT_OFFSET(type,o)  *(type*)(stack+sp+(o))
#define STACK_TOPD_OFFSET(o)        *(double*)(stack+sp+(o))
#define STACK_TOPDT_OFFSET(type,o)  *(type*)(stack+sp+(o))
#define STACK_TOPLD_OFFSET(o)       *(long double*)(stack+sp+(o))
#define STACK_TOPA_OFFSET(o)        (stack+sp)

INTERNAL const char *get_vm_label_name(int index);
INTERNAL void print_vm_instruction(struct vm_program *prog, struct vm_code *code);
INTERNAL void print_vm_instruction_all(struct vm_program *prog);
INTERNAL int vm_run_lir_impl(struct vm_program *prog, int entry, uint8_t *global, int gsize);
INTERNAL int vm_serialize_lir(FILE *fp, struct vm_program *prog);
INTERNAL void vm_import_module(struct vm_context *ctx, struct vm_program *prog, struct vm_program *glbl, String name);

#endif
