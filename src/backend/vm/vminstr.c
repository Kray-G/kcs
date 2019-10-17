#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "../x86_64/abi.h"
#include "vm.h"
#include "vminstr.h"
#include <lacc/context.h>

#include <kcc/assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int is_global_mode = 0;
static int protect_remove_tempvar = 0;
static struct vm_program vm_prog = {0};
static struct vm_program vm_glbl = {0};
static struct vm_context vm_ctx = {0};
static struct vm_func_args_info vm_func_args = {0};
static void *vm_builtin_library = NULL;

typedef const char *(*vm_builtin_get_name_t)(int index);
typedef vm_builtin_t (*vm_builtin_get_func_t)(int index);
static vm_builtin_get_func_t builtin_get_func = NULL;

#define PUSH_ARGS() {\
    for (int p = array_len(&vm_func_args.expr) - 1; 0 <= p ; --p) {\
        struct expression expr = array_get(&vm_func_args.expr, p);\
        vm_gen_expr(expr);\
        int size = size_of(expr.type);\
        vm_func_args.size += PAD8(size);\
    }\
    array_empty(&vm_func_args.expr);\
}\
/**/

#define GEN_2OP_SWAP_CHECK(op) {\
    if (is_swap_available(expr.r)) {\
        vm_load_var(expr.r);\
        vm_load_var(expr.l);\
    }\
    else {\
        vm_load_var(expr.l);\
        vm_load_var(expr.r);\
    }\
    emit_vm_op(op, get_vm_optype(expr.type));\
}\
/**/

#define GEN_2OP_SWAP_CHECK_L(op) {\
    if (is_swap_available(expr.r)) {\
        vm_load_var(expr.r);\
        vm_load_var(expr.l);\
    }\
    else {\
        vm_load_var(expr.l);\
        vm_load_var(expr.r);\
    }\
    emit_vm_op(op, get_vm_optype(expr.l.type));\
}\
/**/

#define GEN_2OP_SWAP_CHECK_CMP(op1, op2) {\
    if (is_swap_available(expr.r)) {\
        vm_load_var(expr.r);\
        vm_load_var(expr.l);\
        emit_vm_op(op2, get_vm_optype(expr.l.type));\
    }\
    else {\
        vm_load_var(expr.l);\
        vm_load_var(expr.r);\
        emit_vm_op(op1, get_vm_optype(expr.l.type));\
    }\
}\
/**/

#define NEXT_BLOCK(i) {\
    if (node->jump[i]->color == BLACK) {\
        emit_vm_jmp(VM_JMP, sym_name(node->jump[i]->label), 0);\
    } else {\
        vm_gen_node(node->jump[i]);\
    }\
}\
/**/

static enum vm_optype get_vmop_var_type(struct var var)
{
    return is_struct_or_union(var.type) ? VMOP_VAROBJ
        : (is_real(var.type) ? VMOP_VARFL : (is_unsigned(var.type) ? VMOP_VARU : VMOP_VARI));
}

static enum vm_optype get_vm_optype(Type type)
{
    /* w = size_of(type); */
    if (is_long_double(type)) {
        return VMOP_LDBL;
    } else if (is_float(type)) {
        return VMOP_FLT;
    } else if (is_double(type)) {
        return VMOP_DBL;
    } else if (is_bool(type) || is_char(type)) {
        return is_unsigned(type) ? VMOP_UINT8 : VMOP_INT8;
    } else if (is_short(type)) {
        return is_unsigned(type) ? VMOP_UINT16 : VMOP_INT16;
    } else if (is_int(type)) {
        return is_unsigned(type) ? VMOP_UINT32 : VMOP_INT32;
    } else if (is_long(type) || is_pointer(type)) {
        return is_unsigned(type) ? VMOP_UINT64 : VMOP_INT64;
    }
    return VMOP_INT32;
}

static struct vm_label *get_vm_label(String name)
{
    int len = array_len(&vm_ctx.labels);
    for (int i = 0; i < len; ++i) {
        struct vm_label *label = &array_get(&vm_ctx.labels, i);
        if (str_cmp(label->name, name) == 0) {
            return label;
        }
    }
    return NULL;
}

static void emit_vm_call(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_CALL,
        .type = var.kind == ADDRESS ? VMOP_FUNCNAME : VMOP_ADDR,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_cleanup(int size)
{
    if (size > 0) {
        emit_vm_code(((struct vm_code){
            .opcode = VM_CLUP,
            .d.size = size,
        }));
    }
}

static void emit_vm_enter(int stack_size)
{
    emit_vm_code(((struct vm_code){
        .opcode = VM_ENTER,
        .d.size = stack_size,
    }));
}

static void emit_vm_ret(int size)
{
    struct vm_code* last = &array_back(&vm_prog.code);
    if (!is_global_mode) {
        if (last->opcode == VM_POP && last->type != VMOP_NONE && !last->d.addr.is_global) {
            /*
             * Don't have to pop it because it means there is no return value.
             * Stack will be adjusted by 'ret' operation like 'mov SP, BP'.
             */
            array_pop_back(&vm_prog.code);
        }
    }
    if (last->opcode == VM_JMP || last->opcode == VM_RET) {
        return;
    }
    emit_vm_code(((struct vm_code){
        .opcode = VM_RET,
        .d.size = size,
    }));
}

static void emit_vm_jmp(enum vm_opcode opcode, const char *label, int size)
{
    struct vm_code* last = is_global_mode ? &array_back(&vm_glbl.code) : &array_back(&vm_prog.code);
    if (last->opcode == VM_JMP) {
        assert(last->opcode != VM_LABEL);
        return;
    }

    emit_vm_code(((struct vm_code){
        .opcode = opcode,
        .d.addr.name = str_init(label),
        .d.addr.size = size,
    }));
}

static void emit_vm_label(const char *name)
{
    int index = array_len(&vm_ctx.labels);
    array_push_back(&vm_ctx.labels, ((struct vm_label){ .index = -1, .name = str_init(name) }));

    emit_vm_code(((struct vm_code){
        .opcode = VM_LABEL,
        .type = VMOP_LABEL,
        .d.lindex = index,
    }));
}

static void emit_vm_push_imm(enum vm_optype type, union vm_imm imm)
{
    emit_vm_code(((struct vm_code){
        .opcode = VM_PUSH,
        .type = type,
        .d.imm = imm,
    }));
}

static void emit_vm_push_string(String string)
{
    int index = vm_ctx.global_index;
    int len = string.len + 1;
    memcpy(vm_prog.global + index, str_raw(string), len);
    vm_ctx.global_index += len;

    union vm_imm imm = { .str = (char*)(vm_prog.global + index) };
    emit_vm_code(((struct vm_code){
        .opcode = VM_PUSH,
        .type = VMOP_CHARP,
        .d.imm = imm,
    }));
}

static void emit_vm_op(enum vm_opcode opcode, enum vm_optype type)
{
    emit_vm_code(((struct vm_code){
        .opcode = opcode,
        .type = type,
    }));
}

static void emit_vm_load_address(struct var var)
{
    if (var.symbol->depth == 0) {
        struct vm_label *label = get_vm_label(var.symbol->name);
        if (label) {
            emit_vm_code(((struct vm_code){
                .opcode = VM_PUSH,
                .type = VMOP_FUNCNAME,
                .d.addr = (struct vm_address){
                    .name = str_init(sym_name(var.symbol)),
                    .is_global = 0,
                    .index = -1,
                    .base = 0,
                    .offset = 0,
                    .size = size_of(var.type),
                },
            }));
            return;
        }
    }

    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    emit_vm_code(((struct vm_code){
        .opcode = VM_PUSH,
        .type = VMOP_ADDR,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size_of(var.type),
        },
    }));
}

static void emit_vm_load(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int type = get_vmop_var_type(var);
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = is_field(var) ? 4 : size_of(var.type);
    if (!protect_remove_tempvar && !is_field(var) && (!is_global_mode || array_len(&vm_glbl.code) > 0)) {
        struct vm_code* last = is_global_mode ? &array_back(&vm_glbl.code) : &array_back(&vm_prog.code);
        if (last->opcode == VM_POP && last->type != VMOP_NONE &&
                (type != VMOP_VARFL || last->type == VMOP_FLT || last->type == VMOP_DBL) &&
                last->d.addr.is_global == is_global_var && last->d.addr.size == size &&
                last->d.addr.base == base && last->d.addr.offset == var.offset) {
            if (is_temporary_var(var)) {
                array_pop_back(&vm_prog.code);  /* temporary var is not global. */
            }
            else {
                last->opcode = VM_STORE;
            }
            return;
        }
    }

    emit_vm_code(((struct vm_code){
        .opcode = VM_PUSH,
        .type = type,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));

    if (is_field(var)) {
        assert(size <= 8);
        if (var.field_offset > 0) {
            emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = var.field_offset });
            emit_vm_code(((struct vm_code){
                .opcode = VM_SHR,
                .type = VMOP_INT32,
            }));
        }
        emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword =  ~(0xFFFFFFFF << var.field_width) });
        emit_vm_code(((struct vm_code){
            .opcode = VM_AND,
            .type = VMOP_UINT32,
        }));
    }
}

static void emit_vm_deref(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    int type = is_struct_or_union(var.type) ? get_vmop_var_type(var) : get_vm_optype(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_DEREF,
        .type = type,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_cast(Type src, Type dst)
{
    emit_vm_code(((struct vm_code){
        .opcode = VM_CAST,
        .type = VMOP_NONE,
        .d.cast.src = src,
        .d.cast.dst = dst,
    }));
}

static void vm_store_bitfield_zero(struct var var)
{
    assert(is_field(var));
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    int type = is_struct_or_union(var.type) ? get_vmop_var_type(var) : get_vm_optype(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_PUSH,
        .type = VMOP_VARU,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
    uint32_t mask = (0xFFFFFFFF << (var.field_width + var.field_offset)) |
                    ~(0xFFFFFFFF << var.field_offset);
    emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword = mask });
    emit_vm_code(((struct vm_code){
        .opcode = VM_AND,
        .type = VMOP_UINT32,
    }));

    emit_vm_code(((struct vm_code){
        .opcode = VM_POP,
        .type = type,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_store(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    int type = is_struct_or_union(var.type) ? get_vmop_var_type(var) : get_vm_optype(var.type);
    if (is_field(var)) {
        size = 4;
        type = VMOP_UINT32; // bit field is used as unsigned 32 bit integer.
        if (var.field_offset > 0) {
            emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = var.field_offset });
            emit_vm_code(((struct vm_code){
                .opcode = VM_SHL,
                .type = VMOP_INT32,
            }));
        }
        emit_vm_code(((struct vm_code){
            .opcode = VM_PUSH,
            .type = VMOP_VARU,
            .d.addr = (struct vm_address){
                .name = str_init(sym_name(var.symbol)),
                .is_global = is_global_var,
                .index = base + var.offset,
                .base = base,
                .offset = var.offset,
                .size = size,
            },
        }));
        uint32_t mask = (0xFFFFFFFF << (var.field_width + var.field_offset)) |
                        ~(0xFFFFFFFF << var.field_offset);
        emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword = mask });
        emit_vm_code(((struct vm_code){
            .opcode = VM_AND,
            .type = VMOP_UINT32,
        }));
        emit_vm_code(((struct vm_code){
            .opcode = VM_OR,
            .type = VMOP_UINT32,
        }));
    }

    emit_vm_code(((struct vm_code){
        .opcode = VM_POP,
        .type = type,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_deref_store(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    int type = is_struct_or_union(var.type) ? get_vmop_var_type(var) : get_vm_optype(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_DEPOP,
        .type = type,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_alloca(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_ALLOCA,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = base + var.offset,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static void emit_vm_pop(struct var var)
{
    int size = size_of(var.type);
    emit_vm_code(((struct vm_code){
        .opcode = VM_POP,
        .type = VMOP_NONE,
        .d.size = size,
    }));
}

static void emit_vm_fill_zero(struct var var)
{
    int is_global_var = var.symbol->global_offset >= 0;
    int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
    int size = size_of(var.type);
    int index = base + var.offset;
    if (!is_global_mode || array_len(&vm_glbl.code) > 0) {
        struct vm_code* last = is_global_mode ? &array_back(&vm_glbl.code) : &array_back(&vm_prog.code);
        if (last->opcode == VM_FZERO && (last->d.addr.index + last->d.addr.size) == index) {
            last->d.addr.size += size;
            return;
        }
    }

    emit_vm_code(((struct vm_code){
        .opcode = VM_FZERO,
        .d.addr = (struct vm_address){
            .name = str_init(sym_name(var.symbol)),
            .is_global = is_global_var,
            .index = index,
            .base = base,
            .offset = var.offset,
            .size = size,
        },
    }));
}

static int is_constant_imm(struct expression expr)
{
    return expr.op == IR_OP_CAST &&
        expr.l.kind == IMMEDIATE &&
        is_integer(expr.l.type);
}

static int is_constant_zero(struct expression expr)
{
    return expr.op == IR_OP_CAST &&
        expr.l.kind == IMMEDIATE &&
        is_integer(expr.l.type) &&
        expr.l.imm.i == 0;
}

static void vm_load_var(const struct var var)
{
    int n = 0;
    struct var ptr;

    switch (var.kind) {
    case IMMEDIATE:
        switch (type_of(var.type)) {
        default: assert(0);
        case T_POINTER:
            if (var.symbol) {
                emit_vm_push_string(var.symbol->value.string);
                assert(var.symbol->symtype == SYM_STRING_VALUE);
                if (var.offset) {
                    emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = var.offset });
                    emit_vm_op(VM_ADD, VMOP_UINT64);
                }
                break;
            }
        case T_BOOL:
        case T_CHAR:
            if (is_unsigned(var.type)) {
                emit_vm_push_imm(VMOP_UINT8, (union vm_imm){ .byte = var.imm.u });
            } else {
                emit_vm_push_imm(VMOP_INT8, (union vm_imm){ .byte = var.imm.i });
            }
            break;
        case T_SHORT:
            if (is_unsigned(var.type)) {
                emit_vm_push_imm(VMOP_UINT16, (union vm_imm){ .word = var.imm.u });
            } else {
                emit_vm_push_imm(VMOP_INT16, (union vm_imm){ .word = var.imm.i });
            }
            break;
        case T_INT:
            if (is_unsigned(var.type)) {
                emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword = var.imm.u });
            } else {
                emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = var.imm.i });
            }
            break;
        case T_LONG:
            if (is_unsigned(var.type)) {
                emit_vm_push_imm(VMOP_UINT64, (union vm_imm){ .qword = var.imm.u });
            } else {
                emit_vm_push_imm(VMOP_INT64, (union vm_imm){ .qword = var.imm.i });
            }
            break;
        case T_FLOAT:
            emit_vm_push_imm(VMOP_FLT, (union vm_imm){ .f = var.imm.f });
            break;
        case T_DOUBLE:
            emit_vm_push_imm(VMOP_DBL, (union vm_imm){ .d = var.imm.d });
            break;
        case T_LDOUBLE:
            emit_vm_push_imm(VMOP_LDBL, (union vm_imm){ .ld = var.imm.ld });
            break;
        case T_ARRAY:
            emit_vm_push_string(var.symbol->value.string);
            break;
        }
        break;
    case DIRECT:
        emit_vm_load(var);
        break;
    case ADDRESS:
        emit_vm_load_address(var);
        break;
    case DEREF:
        ptr = var_direct(var.symbol);
        if (is_string_var(ptr)) {
            emit_vm_push_string(var.symbol->value.string);
        } else {
            protect_remove_tempvar = 1;
            emit_vm_load(ptr);
            protect_remove_tempvar = 0;
        }
        if (var.offset) {
            emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = var.offset });
            emit_vm_op(VM_ADD, VMOP_UINT64);
        }
        emit_vm_deref(var);
        break;
    }
}

static void vm_store_var(const struct var var)
{
    switch (var.kind) {
    case DIRECT:
        emit_vm_store(var);
        break;
    case DEREF: {
        emit_vm_deref_store(var);
        break;
    }
    case IMMEDIATE:
    case ADDRESS:
        assert(0);
        break;
    }
}

static int is_swap_available(struct var var)
{
    if (var.symbol && array_len(&vm_prog.code) && (!is_global_mode || array_len(&vm_glbl.code) > 0)) {
        int is_global_var = var.symbol->global_offset >= 0;
        int base = is_global_var ? var.symbol->global_offset : var.symbol->stack_offset;
        int size = size_of(var.type);
        struct vm_code* last = is_global_mode ? &array_back(&vm_glbl.code) : &array_back(&vm_prog.code);
        if (last && last->opcode == VM_POP && last->d.addr.is_global == is_global_var && last->d.addr.size == size &&
            last->d.addr.base == base && last->d.addr.offset == var.offset) {
            return 1;
        }
    }
    return 0;
}

static void vm_gen_expr(struct expression expr)
{
    switch (expr.op) {
    case IR_OP_CAST:
        if (type_equal(expr.type, expr.l.type)) {
            vm_load_var(expr.l);
        } else {
            vm_load_var(expr.l);
            emit_vm_cast(expr.l.type, expr.type);
        }
        break;
    case IR_OP_CALL: {
        emit_vm_call(expr.l);
        emit_vm_cleanup(vm_func_args.size);
        vm_func_args.size = 0;
        break;
    }
    case IR_OP_VA_ARG:
        /*
         * VM do not use this because va_*** is just defined as macros.
         * Compiler will not handle this IR.
         */
        assert(0);
        break;
    case IR_OP_NOT:
        vm_load_var(expr.l);
        emit_vm_op(VM_NOT, get_vm_optype(expr.l.type));
        break;
    case IR_OP_NEG:
        vm_load_var(expr.l);
        emit_vm_op(VM_NEG, get_vm_optype(expr.l.type));
        break;
    case IR_OP_ADD:
        GEN_2OP_SWAP_CHECK(VM_ADD);
        break;
    case IR_OP_SUB:
        vm_load_var(expr.l);
        vm_load_var(expr.r);
        emit_vm_op(VM_SUB, get_vm_optype(expr.type));
        break;
    case IR_OP_MUL:
        GEN_2OP_SWAP_CHECK(VM_MUL);
        break;
    case IR_OP_DIV:
        vm_load_var(expr.l);
        vm_load_var(expr.r);
        emit_vm_op(VM_DIV, get_vm_optype(expr.type));
        break;
    case IR_OP_MOD:
        vm_load_var(expr.l);
        vm_load_var(expr.r);
        emit_vm_op(VM_MOD, get_vm_optype(expr.type));
        break;
    case IR_OP_AND:
        GEN_2OP_SWAP_CHECK(VM_AND);
        break;
    case IR_OP_OR:
        GEN_2OP_SWAP_CHECK(VM_OR);
        break;
    case IR_OP_XOR:
        GEN_2OP_SWAP_CHECK(VM_XOR);
        break;
    case IR_OP_SHL:
        vm_load_var(expr.l);
        vm_load_var(expr.r);
        emit_vm_op(VM_SHL, get_vm_optype(expr.type));
        break;
    case IR_OP_SHR:
        vm_load_var(expr.l);
        vm_load_var(expr.r);
        emit_vm_op(VM_SHR, get_vm_optype(expr.type));
        break;
    case IR_OP_EQ:
        if (is_real(expr.l.type) && is_real(expr.r.type)) {
            GEN_2OP_SWAP_CHECK_L(VM_EQ);
        } else {
            GEN_2OP_SWAP_CHECK(VM_EQ);
        }
        break;
    case IR_OP_NE:
        if (is_real(expr.l.type) && is_real(expr.r.type)) {
            GEN_2OP_SWAP_CHECK_L(VM_NE);
        } else {
            GEN_2OP_SWAP_CHECK(VM_NE);
        }
        break;
    case IR_OP_GE:
        GEN_2OP_SWAP_CHECK_CMP(VM_GE, VM_LE);
        break;
    case IR_OP_GT:
        GEN_2OP_SWAP_CHECK_CMP(VM_GT, VM_LT);
        break;
    }
}

static void vm_gen_string_array(struct var var, struct expression expr)
{
    const char *str = str_raw(expr.l.symbol->value.string);
    struct var target = var;
    size_t size = size_of(target.type);
    target.type = type_next(target.type);
    for (int i = 0; i < size / size_of(target.type); ++i) {
        target.offset = var.offset + i * size_of(target.type);
        emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = str[i] });
        vm_store_var(target);
    }
}

static void vm_gen_node(struct block *node)
{
    int i;
    struct statement s;

    if (node->color == BLACK)
        return;

    node->color = BLACK;
    emit_vm_label(sym_name(node->label));

    if (node->body) {
        assert(node->jump[0]);
        assert(!node->jump[1]);
        emit_vm_jmp(VM_JMP, sym_name(node->jump[0]->label), size_of(node->expr.type));
        vm_gen_node(node->body);
    }

    for (i = 0; i < array_len(&node->code); ++i) {
        s = array_get(&node->code, i);
        if (array_len(&vm_func_args.expr) > 0 && s.st != IR_PARAM) {
            PUSH_ARGS();
        }
        switch (s.st) {
        case IR_ASSIGN:
            if (!is_field(s.t) && !is_void(s.t.type) && is_constant_zero(s.expr) && s.t.kind != DEREF) {
                emit_vm_fill_zero(s.t);
            } else if (is_array(s.t.type) && is_array(s.expr.type) && s.expr.op == IR_OP_CAST) {
                assert(s.expr.l.symbol);
                vm_gen_string_array(s.t, s.expr);
            } else {
                /* workaround */
                if (is_field(s.t) && is_constant_imm(s.expr)) {
                    uint32_t mask = ~(0xFFFFFFFF << s.t.field_width);
                    if (s.expr.l.imm.u == 0) {
                        vm_store_bitfield_zero(s.t);
                    } else {
                        protect_remove_tempvar = is_temporary_var(s.t) && s.t.kind == DEREF;
                        emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword = s.expr.l.imm.u & mask });
                        vm_store_var(s.t);
                        protect_remove_tempvar = 0;
                    }
                } else {
                    protect_remove_tempvar = is_temporary_var(s.t) && s.t.kind == DEREF;
                    vm_gen_expr(s.expr);
                    vm_store_var(s.t);
                    protect_remove_tempvar = 0;
                }
            }
            break;
        case IR_PARAM:
            /* push vm_func_args from right side. */
            array_push_back(&vm_func_args.expr, s.expr);
            break;
        case IR_VA_START:
            /*
            * VM do not use this because va_*** is just defined as macros.
            * Compiler will not handle this IR.
            */
            // assert(0);
            break;
        case IR_EXPR:
            vm_gen_expr(s.expr);
            emit_vm_pop(s.expr.l);
            break;
        case IR_VLA_ALLOC:
            vm_gen_expr(s.expr);
            emit_vm_alloca(var_direct(s.t.symbol->value.vla_address));
            break;
        }
    }
    if (array_len(&vm_func_args.expr) > 0) {
        PUSH_ARGS();
    }

    if (node->body) {
        vm_gen_node(node->jump[0]);
    } else if (!node->jump[0] && !node->jump[1]) {
        if (node->has_return_value) {
            vm_gen_expr(node->expr);
            emit_vm_ret(size_of(node->expr.type));
        } else {
            emit_vm_ret(0);
        }
    } else if (node->jump[1]) {
        assert(node->jump[0]);
        vm_gen_expr(node->expr);
        emit_vm_jmp(VM_JNZ, sym_name(node->jump[1]->label), size_of(node->expr.type));
        NEXT_BLOCK(0);
        NEXT_BLOCK(1);
    } else {
        assert(node->jump[0]);
        assert(!node->jump[1]);
        NEXT_BLOCK(0);
    }
}

static void vm_gen_data(struct definition *def)
{
    struct statement st;
    size_t total_size = size_of(def->symbol->type), initialized = 0;

    for (int i = 0; i < array_len(&def->body->code); ++i) {
        st = array_get(&def->body->code, i);
        initialized = st.t.offset;
        is_global_mode = st.t.symbol->global_offset >= 0;
        if (!is_field(st.t)) {
            assert(st.st == IR_ASSIGN);
            assert(st.t.kind == DIRECT);
            assert(is_identity(st.expr));
            assert(st.t.symbol == def->symbol);
            if (is_constant_zero(st.expr)) {
                emit_vm_fill_zero(st.t);
            }
            else if (is_array(st.t.type) && is_array(st.expr.type) && st.expr.op == IR_OP_CAST) {
                assert(st.expr.l.symbol);
                vm_gen_string_array(st.t, st.expr);
            }
            else {
                vm_gen_expr(st.expr);
                vm_store_var(st.t);
            }
        } else {
            if (is_constant_imm(st.expr)) {
                uint32_t mask = ~(0xFFFFFFFF << st.t.field_width);
                if (st.expr.l.imm.u == 0) {
                    vm_store_bitfield_zero(st.t);
                } else {
                    emit_vm_push_imm(VMOP_UINT32, (union vm_imm){ .dword = st.expr.l.imm.u & mask });
                    vm_store_var(st.t);
                }
            } else {
                vm_gen_expr(st.expr);
                vm_store_var(st.t);
            }
        }
        is_global_mode = 0;
        initialized = st.t.offset + size_of(st.t.type);
    }
}

static int vm_allocate_locals(struct definition *def)
{
    int i, stack_offset = 0;
    struct symbol *sym;

    for (i = 0; i < array_len(&def->locals); ++i) {
        sym = array_get(&def->locals, i);
        assert(!sym->stack_offset);
        assert(sym->symtype == SYM_DEFINITION);
        if (sym->linkage == LINK_NONE && sym->slot == 0 && !is_vla(sym->type)) {
            sym->stack_offset = stack_offset;
            stack_offset += EIGHTBYTES(sym->type) * 8;
        }
    }

    return stack_offset;
}

static void vm_func_enter(struct definition *def)
{
    emit_vm_label(sym_name(def->symbol));

    int i, n, mem_offset = -16;
    struct symbol *sym;
    struct param_class arg;
    for (i = 0; i < array_len(&def->params); ++i) {
        sym = array_get(&def->params, i);
        arg = classify(sym->type);
        n = EIGHTBYTES(sym->type);
        assert(!sym->stack_offset);
        assert(sym->linkage == LINK_NONE);
        mem_offset -= n * 8;
        sym->stack_offset = mem_offset;
    }

    vm_ctx.stack_offset = vm_allocate_locals(def);
    emit_vm_enter(vm_ctx.stack_offset);
}

static void update_label_index()
{
    int index = 0;
    int len = array_len(&vm_prog.code);
    for (int i = 0; i < len; ++i) {
        struct vm_code *code = &array_get(&vm_prog.code, i);
        if (code->opcode != VM_LABEL) {
            code->index = index++;
            array_push_back(&vm_prog.exec, code);
        }
        else {
            // VM_LABEL.
            code->index = index;    // Without updating the index.
            if (code->d.lindex >= 0) {
                struct vm_label *label = &array_get(&vm_ctx.labels, code->d.lindex);
                label->index = index;
            }
        }
    }
}

static int get_global_offset(String name)
{
    int len = array_len(&vm_ctx.globals);
    for (int i = 0; i < len; ++i) {
        struct vm_label* gl = &array_get(&vm_ctx.globals, i);
        if (!str_cmp(gl->name, name)) {
            return gl->index;
        }
    }
    return 0;
}

static void jump_optimization()
{
    int len = array_len(&vm_prog.exec);
    for (int i = 0; i < len; ++i) {
        struct vm_code *code = array_get(&vm_prog.exec, i);
        switch (code->opcode) {
        case VM_JMP:
        case VM_JZ:
        case VM_JNZ: {
            int index = i + code->d.addr.index;
            if (index < len) {
                struct vm_code *prev = NULL;
                struct vm_code *dst = array_get(&vm_prog.exec, index);
                while (dst->opcode == VM_JMP) {
                    prev = dst;
                    index += prev->d.addr.index;
                    if (len <= index) {
                        break;
                    }
                    dst = array_get(&vm_prog.exec, index);
                }
                if (prev) {
                    code->d.addr.index = index - i;
                }
                if (code->d.addr.index == 1) {
                    code->opcode = VM_NOP;
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

static void reassign_label_index()
{
    update_label_index();
    int len = array_len(&vm_prog.exec);
    for (int i = 0; i < len; ++i) {
        struct vm_code *code = array_get(&vm_prog.exec, i);
        switch (code->opcode) {
        case VM_JMP:
        case VM_JZ:
        case VM_JNZ: {
            if (code->d.addr.index == 0) {
                struct vm_label *label = get_vm_label(code->d.addr.name);
                assert(label);
                assert(label->index > 0);
                code->d.addr.index = label->index - i;
            }
            break;
        }
        case VM_STORE:
        case VM_FZERO:
        case VM_DEREF:
        case VM_DEPOP:
            if (code->d.addr.is_global) {
                code->d.addr.index +=  get_global_offset(code->d.addr.name);
            }
            break;
        case VM_POP:
            if (code->type != VMOP_NONE) {
                if (code->d.addr.is_global) {
                    code->d.addr.index +=  get_global_offset(code->d.addr.name);
                }
            }
            break;
        case VM_PUSH:
            if (code->type == VMOP_ADDR || code->type == VMOP_VARI || code->type == VMOP_VARU || code->type == VMOP_VAROBJ || code->type == VMOP_VARFL) {
                if (code->d.addr.is_global) {
                    code->d.addr.index +=  get_global_offset(code->d.addr.name);
                }
            }
            if (code->type == VMOP_ADDR && code->d.addr.is_global) {
                struct vm_label *label = get_vm_label(code->d.addr.name);
                if (label) {
                    code->type = VMOP_FUNCNAME;
                }
            }
            // fall through.
        case VM_CALL:
            if (code->type == VMOP_FUNCNAME) {
                struct vm_label *label = get_vm_label(code->d.addr.name);
                if (!label) {
                    if (context.target != TARGET_IR_SAVE) {
                        error("Function not found: %s\n", str_raw(code->d.addr.name));
                    }
                } else if (label->index < 0) {
                    if (code->opcode == VM_PUSH) {
                        error("Storing a built-in function into a variable is not allowed: %s\n", str_raw(code->d.addr.name));
                        exit(1);
                    }
                    code->type = VMOP_BUILTIN;
                    code->d.addr.func = builtin_get_func(-(label->index));
                } else {
                    code->type = VMOP_FUNCADDR;
                    code->d.addr.index = label->index;
                }
            }
            break;
        }
    }
    jump_optimization();
}

static void vm_fix_lir(void)
{
    // array_len(&vm_ctx.imports) might be changed dynamically by VM_REFLIB.
    for (int i = 0; i < array_len(&vm_ctx.imports); ++i) {
        String module = array_get(&vm_ctx.imports, i);
        vm_import_module(&vm_ctx, &vm_prog, &vm_glbl, module);
    }

    is_global_mode = 1;
    emit_vm_code(((struct vm_code){
        .opcode = VM_CALL,
        .type = VMOP_FUNCNAME,
        .d.addr = (struct vm_address){
            .name = str_init("__kcc_builtin_onstart"),
            .index = -1,
        },
    }));
    emit_vm_push_imm(VMOP_UINT64, (union vm_imm){ .qword = (uint64_t)kcc_argv + (kcc_argx*8) });
    emit_vm_push_imm(VMOP_INT32, (union vm_imm){ .dword = kcc_argc - kcc_argx });
    emit_vm_code(((struct vm_code){
        .opcode = VM_CALL,
        .type = VMOP_FUNCNAME,
        .d.addr = (struct vm_address){
            .name = str_init("main"),
            .index = -1,
        },
    }));
    emit_vm_code(((struct vm_code){
        .opcode = VM_SAVE_RETVAL,
    }));
    emit_vm_code(((struct vm_code){
        .opcode = VM_CALL,
        .type = VMOP_FUNCNAME,
        .d.addr = (struct vm_address){
            .name = str_init("__kcc_call_atexit_funcs"),
            .index = -1,
        },
    }));
    emit_vm_code(((struct vm_code){
        .opcode = VM_CALL,
        .type = VMOP_FUNCNAME,
        .d.addr = (struct vm_address){
            .name = str_init("__kcc_builtin_onexit"),
            .index = -1,
        },
    }));
    emit_vm_code(((struct vm_code){ .opcode = VM_HALT }));
    is_global_mode = 0;

    /* update global variable size. */
    int size = get_global_variable_size();
    struct vm_code *code = &array_get(&vm_glbl.code, 1);
    code->d.size = PAD8(size);

    array_concat(&vm_prog.code, &vm_glbl.code);
    reassign_label_index();
}

static void vm_setup_builtin(int index, const char *name)
{
    array_push_back(&vm_ctx.labels, ((struct vm_label){ .index = index, .name = str_init(name) }));
}

INTERNAL const char *get_vm_label_name(int index)
{
    if (index < 0) {
        return ".L$";
    }
    return str_raw(array_get(&vm_ctx.labels, index).name);
}

INTERNAL void add_import_module(const char *name)
{
    for (int i = 0; i < array_len(&vm_ctx.imports); ++i) {
        String module = array_get(&vm_ctx.imports, i);
        if (strcmp(str_raw(module), name) == 0) {
            /*  already exists */
            return;
        }
    }
    array_push_back(&vm_ctx.imports, str_init(name));
}

INTERNAL void add_ref_module(const char *name)
{
    emit_vm_code(((struct vm_code){ .opcode = VM_REFLIB, .d.name = str_init(name) }));
}

INTERNAL void vm_init(FILE *stream, const char *file)
{
    int is_save = (file != NULL);
    vm_ctx.stream = stream;
    vm_ctx.file = file;

    assert(!vm_prog.global);
    vm_builtin_library = load_library("kccbltin", 0);
    if (!vm_builtin_library) return;
    builtin_get_func = (vm_builtin_get_func_t)get_function(vm_builtin_library, "vm_get_builtin_by_index");
    if (!builtin_get_func) return;
    vm_builtin_get_name_t builtin_get_name =
        (vm_builtin_get_name_t)get_function(vm_builtin_library, "vm_get_builtin_name_by_index");
    if (!builtin_get_name) return;
    for (int i = 1; ; ++i) {
        const char *name = builtin_get_name(i);
        if (!name) break;
        vm_setup_builtin(-i, name);
    }

    vm_prog.global = calloc(1, VM_GLOBAL_MEM_SIZE);
    if (!is_save) {
        emit_vm_label("_startup");
        emit_vm_jmp(VM_JMP, "_setup_global", 0);
    }

    is_global_mode = 1;
    emit_vm_label("_setup_global");
    assert(array_len(&vm_glbl.code) == 1);
    emit_vm_enter(0);   /* update it by actual global variable size later. */
    is_global_mode = 0;
}

INTERNAL void vm_gen_lir(struct definition *def)
{
    if (is_function(def->symbol->type)) {
        vm_func_enter(def);
        vm_gen_node(def->body);
        struct vm_code* last = &array_back(&vm_prog.code);
        if (last->opcode != VM_RET) {
            emit_vm_ret(0);
        }
    }
    else {
        vm_gen_data(def);
    }
}

INTERNAL int vm_run_lir(void)
{
    vm_fix_lir();
    vm_run_lir_impl(&vm_prog, 0, vm_prog.global, vm_ctx.global_index);
    return 0;
}

INTERNAL int vm_print_lir(void)
{
    vm_fix_lir();
    print_vm_instruction_all(&vm_prog);
    return 0;
}

INTERNAL int vm_save_lir(void)
{
    /* update global variable size. */
    int size = get_global_variable_size();
    struct vm_code *code = &array_get(&vm_glbl.code, 1);
    code->d.size = PAD8(size);

    array_concat(&vm_prog.code, &vm_glbl.code);
    reassign_label_index();
    // print_vm_instruction_all(&vm_prog);
    vm_serialize_lir(vm_ctx.stream, &vm_prog);
    return 0;
}

INTERNAL int vm_finalize_lir(void)
{
    array_clear(&vm_func_args.expr);
    array_clear(&vm_ctx.globals);
    array_clear(&vm_ctx.labels);
    array_clear(&vm_ctx.imports);
    array_clear(&vm_prog.code);
    array_clear(&vm_prog.exec);
    array_clear(&vm_glbl.code);
    array_clear(&vm_glbl.exec);
    free(vm_prog.global);
    if (vm_builtin_library) unload_library(vm_builtin_library);
    return 0;
}
