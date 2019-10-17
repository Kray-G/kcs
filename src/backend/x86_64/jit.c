#include <kcc.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "abi.h"
#include "elf.h"
#include "dwarf.h"
#include "assemble.h"
#include "jit_util.h"
#include "jit.h"
#include <lacc/array.h>
#include <lacc/context.h>
#include <kcc/assert.h>

#define JIT_ADDR_BASE (0)
static int jit_return_value = 0;

struct jit_code {
    int is_float_value  : 1;
    int is_double_value : 1;
    int is_string_value : 1;
    int is_ascii_value  : 1;
    int is_address_value: 1;
    int is_label_ref    : 1;
    int is_label_value  : 1;
    int label_hidden    : 1;
    int int_value_size;
    String name;
    int addr;
    int base;
    struct code code;
    struct instruction instr;
    String label_text;
    union {
        uint64_t u;
        int64_t i;
    } value;
};

struct jit_label {
    String name;
    int index;
    void *builtin;
    uint8_t flbit;
    uint8_t args;
};

struct jit_context {
    int passed;
    void *buffer;
    int size;
    array_of(struct jit_label) labels;
    array_of(struct jit_code) jcode;
};

static struct jit_context jit;
static int jit_addr = 0;
static const struct symbol *jit_sym_curr = NULL;
static const struct symbol *jit_sym_prev = NULL;

static void *jit_get_builtin_address(int index)
{
    assert(index < 0);
    for (int i = 0; i < array_len(&jit.labels); ++i) {
        struct jit_label *l = &array_get(&jit.labels, i);
        if (index == l->index) {
            return l->builtin;
        }
    }
    return 0;
}

static int jit_get_label_address(const char *label)
{
    if (!label) {
        return -1;
    }
    for (int i = 0; i < array_len(&jit.labels); ++i) {
        struct jit_label *l = &array_get(&jit.labels, i);
        if (!strcmp(str_raw(l->name), label)) {
            return l->index;
        }
    }
    return -1;
}

INTERNAL void *jit_get_builtin_function(const char *label)
{
    for (int i = 0; i < array_len(&jit.labels); ++i) {
        struct jit_label *l = &array_get(&jit.labels, i);
        if (!strcmp(str_raw(l->name), label) && l->index < 0) {
            return l->builtin;
        }
    }
    return NULL;
}

INTERNAL uint8_t jit_get_builtin_flbit(const char *label)
{
    for (int i = 0; i < array_len(&jit.labels); ++i) {
        struct jit_label *l = &array_get(&jit.labels, i);
        if (!strcmp(str_raw(l->name), label) && l->index < 0) {
            return l->flbit;
        }
    }
    return 0;
}

INTERNAL uint8_t jit_get_builtin_args(const char *label)
{
    for (int i = 0; i < array_len(&jit.labels); ++i) {
        struct jit_label *l = &array_get(&jit.labels, i);
        if (!strcmp(str_raw(l->name), label) && l->index < 0) {
            return l->args;
        }
    }
    return 0;
}


static void *jit_builtin_library = NULL;

typedef const char *(*jit_builtin_get_name_t)(int index);
typedef void *(*jit_builtin_get_func_t)(int index);
typedef  uint8_t (*jit_builtin_get_args_t)(int index);
typedef  uint8_t (*jit_builtin_get_flbit_t)(int index);
static jit_builtin_get_func_t builtin_get_func = NULL;

static void jit_setup_builtin(void)
{
    jit_builtin_library = load_library("kccjit", 0);
    if (!jit_builtin_library) return;
    builtin_get_func = (jit_builtin_get_func_t)get_function(jit_builtin_library, "jit_get_builtin_by_index");
    if (!builtin_get_func) return;
    jit_builtin_get_name_t builtin_get_name =
        (jit_builtin_get_name_t)get_function(jit_builtin_library, "jit_get_builtin_name_by_index");
    if (!builtin_get_name) return;
    jit_builtin_get_flbit_t builtin_get_flbit =
        (jit_builtin_get_flbit_t)get_function(jit_builtin_library, "jit_get_builtin_flbit_by_index");
    if (!builtin_get_flbit) return;
    jit_builtin_get_args_t builtin_get_args =
        (jit_builtin_get_args_t)get_function(jit_builtin_library, "jit_get_builtin_args_by_index");
    if (!builtin_get_args) return;

    for (int i = 1; ; ++i) {
        const char *name = builtin_get_name(i);
        if (!name) break;
        void *func =  builtin_get_func(i);
        uint8_t args = builtin_get_args(i);
        uint8_t flbit = builtin_get_flbit(i);
        array_push_back(&jit.labels, ((struct jit_label){
            .name = str_init(name),
            .index = -i,
            .builtin = func,
            .args = args,
            .flbit = flbit,
        }));
    }
}

static void jit_update_jump(int len)
{
    for (int i = jit.passed; i < len; ++i) {
        struct jit_code *jc = &array_get(&jit.jcode, i);
        if (jc->is_label_ref) {
            if (jc->is_address_value) {
                int laddr = jit_get_label_address(str_raw(jc->label_text));
                jc->value.u += (uint64_t)jit.buffer + laddr + JIT_ADDR_BASE;
            } else if (jc->code.len > 4) {
                int laddr = jit_get_label_address(str_raw(jc->label_text));
                if (laddr < 0) {
                    laddr = jit_get_label_address(str_raw(jc->name));
                }
                if (laddr < 0) {
                    continue;
                }
                int n;
                int saddr = jc->base;
                // printf("%s: base:%x -> label:%x\n", str_raw(jc->label_text), saddr, laddr);
                uint32_t d = (uint32_t)(laddr - saddr);
                if (jc->instr.optype == OPT_IMM_MEM || jc->instr.optype == OPT_REG_MEM) {
                    // dst.
                    if (jc->code.len >= 8 && (jc->code.val[0] == 0xF2 || jc->code.val[0] == 0xF3) && jc->code.val[1] == 0x0F) {
                        n = 4;
                    } else {
                        n = (jc->code.val[0] == 0x66 || (0x48 <= jc->code.val[0] && jc->code.val[0] <= 0x4F)) ? 3 : 2;
                    }
                    if (jc->instr.dest.mem.addr.disp) {
                        d += jc->instr.dest.mem.addr.disp;
                    }
                } else {
                    // src.
                    n = jc->code.len - 4;
                    if (jc->instr.optype == OPT_MEM_REG) {
                        if (jc->instr.source.mem.addr.disp) {
                            d += jc->instr.source.mem.addr.disp;
                        }
                    }
                }
                jc->code.val[n++] = (d      ) & 0xFF;
                jc->code.val[n++] = (d >>  8) & 0xFF;
                jc->code.val[n++] = (d >> 16) & 0xFF;
                jc->code.val[n  ] = (d >> 24) & 0xFF;
            }
        }
    }
}

static void jit_print_code()
{
    for (int i = 0; i < array_len(&jit.jcode); ++i) {
        struct jit_code *jc = &array_get(&jit.jcode, i);
        if (jc->is_label_value) {
            printf("%34s%s\n", "", str_raw(jc->name));
        } else if (jc->is_string_value) {
            if (!jc->label_hidden) {
                printf("%33s%s\n", "", str_raw(jc->name));
            }
            printf("%08X:", jc->addr);
            String s = jc->label_text;
            int len = s.len + 1;
            const char *str = str_raw(s);
            for (int i = 0; i < len; ++i) {
                printf(" %02X", str[i]);
            }
            for (int i = len; i < 8; ++i) {
                printf("   ");
            }
            printf("\t%s\n", str);
        } else if (jc->is_ascii_value) {
            if (!jc->label_hidden) {
                printf("%33s%s\n", "", str_raw(jc->name));
            }
            printf("%08X:", jc->addr);
            String s = jc->label_text;
            int len = s.len;
            const char *str = str_raw(s);
            for (int i = 0; i < len; ++i) {
                printf(" %02X", str[i]);
            }
            for (int i = len; i < 8; ++i) {
                printf("   ");
            }
            printf("\t(ascii)\n");
        } else if (jc->int_value_size > 0) {
            if (!jc->label_hidden) {
                printf("%33s%s\n", "", str_raw(jc->label_text));
            }
            printf("%08X:", jc->addr);
            uint64_t value = jc->value.u;
            for (int i = 0; i < jc->int_value_size; ++i) {
                printf(" %02X", value & 0xFF);
                value = value >> 8;
            }
            if (jc->int_value_size > 8) {
                printf("\n");
            } else {
                printf("\t%llu\n", jc->value.u);
            }
        } else if (jc->is_float_value) {
            printf("%33s%s\n", "", str_raw(jc->label_text));
            printf("%08X:", jc->addr);
            printf(" %02X", (jc->value.u      ) & 0xFF);
            printf(" %02X", (jc->value.u >>  8) & 0xFF);
            printf(" %02X", (jc->value.u >> 16) & 0xFF);
            printf(" %02X", (jc->value.u >> 24) & 0xFF);
            for (int i = 0; i < 4; ++i) {
                printf("   ");
            }
            printf("\t%llu\n", jc->value.u);
        } else if (jc->is_double_value) {
            printf("%33s%s\n", "", str_raw(jc->label_text));
            printf("%08X:", jc->addr);
            printf(" %02X", (jc->value.i      ) & 0xFF);
            printf(" %02X", (jc->value.i >>  8) & 0xFF);
            printf(" %02X", (jc->value.i >> 16) & 0xFF);
            printf(" %02X", (jc->value.i >> 24) & 0xFF);
            printf(" %02X", (jc->value.i >> 32) & 0xFF);
            printf(" %02X", (jc->value.i >> 40) & 0xFF);
            printf(" %02X", (jc->value.i >> 48) & 0xFF);
            printf(" %02X", (jc->value.i >> 56) & 0xFF);
            printf("\t%lld\n", jc->value.i);
        } else if (jc->is_address_value) {
            if (!jc->label_hidden) {
                printf("%33s%s\n", "", str_raw(jc->name));
            }
            printf("%08X:", jc->addr);
            uint64_t value = jc->value.u;
            printf(" %02X", value & 0xFF);
            for (int i = 0; i < 7; ++i) {
                value = value >> 8;
                printf(" %02X", value & 0xFF);
            }
            printf("\t%llu\n", jc->value.u);
        } else {
            struct code c = jc->code;
            printf("%08X:", jc->addr);
            for (int i = 0; i < c.len; ++i) {
                printf(" %02X", c.val[i]);
            }
            for (int i = c.len; i < 8; ++i) {
                printf("   ");
            }
            asm_text(jc->instr);
        }
    }
}

static void jit_update_code()
{
    int len = array_len(&jit.jcode);
    jit_update_jump(len);
    jit.passed = len;
}

static int jit_fix_code(void)
{
    jit.size = PAD8(jit_addr + 16);
    jit_create(&jit.buffer, jit.size);
    if (!jit.buffer) {
        return 1;
    }

    jit_update_code();
    int main_found = 0;
    int s = JIT_ADDR_BASE;
    uint8_t *p = (char*)jit.buffer;
    /* A jmp placeholder to main. */
    int len = array_len(&jit.jcode);
    for (int i = 0; i < len; ++i) {
        struct jit_code *jc = &array_get(&jit.jcode, i);
        if (jc->is_string_value) {
            const char *str = str_raw(jc->label_text);
            int len = jc->label_text.len + 1;
            for (int i = 0; i < len; ++i) {
                *p++ = str[i];
            }
            s += len;
        } else if (jc->is_ascii_value) {
            const char *str = str_raw(jc->label_text);
            int len = jc->label_text.len;
            for (int i = 0; i < len; ++i) {
                *p++ = str[i];
            }
            s += len;
        } else if (jc->int_value_size > 0) {
            uint64_t value = jc->value.u;
            for (int i = 0; i < jc->int_value_size; ++i) {
                *p++ = value & 0xFF;
                value = value >> 8;
            }
        } else if (jc->is_float_value) {
            *p++ = (jc->value.u      ) & 0xFF;
            *p++ = (jc->value.u >>  8) & 0xFF;
            *p++ = (jc->value.u >> 16) & 0xFF;
            *p++ = (jc->value.u >> 24) & 0xFF;
            s += 4;
        } else if (jc->is_double_value) {
            *p++ = (jc->value.i      ) & 0xFF;
            *p++ = (jc->value.i >>  8) & 0xFF;
            *p++ = (jc->value.i >> 16) & 0xFF;
            *p++ = (jc->value.i >> 24) & 0xFF;
            *p++ = (jc->value.i >> 32) & 0xFF;
            *p++ = (jc->value.i >> 40) & 0xFF;
            *p++ = (jc->value.i >> 48) & 0xFF;
            *p++ = (jc->value.i >> 56) & 0xFF;
            s += 8;
        } else if (!strcmp(str_raw(jc->name), "main")) {
            main_found = 1;
            uint8_t *px = (uint8_t*)jit.buffer;
            int d = p - (uint8_t*)jit.buffer - 5;
            struct jit_code *mc = &array_get(&jit.jcode, 0);
            mc->instr.source.imm.d.dword = d;
            mc->code.val[1] = *++px = (d      ) & 0xFF;
            mc->code.val[2] = *++px = (d >>  8) & 0xFF;
            mc->code.val[3] = *++px = (d >> 16) & 0xFF;
            mc->code.val[4] = *++px = (d >> 24) & 0xFF;
        } else if (jc->is_address_value) {
            uint64_t value = jc->value.u;
            *p++ = value & 0xFF;
            for (int i = 0; i < 7; ++i) {
                value = value >> 8;
                *p++ = value & 0xFF;
            }
        } else if (jc->code.len > 0) {
            struct code c = jc->code;
            for (int i = 0; i < c.len; ++i) {
                *p++ = c.val[i];
            }
            s += c.len;
        }
    }

    return main_found;
}

static void jit_gen_label(int index, String name)
{
    array_push_back(&jit.labels, ((struct jit_label){
        .name = name,
        .index = index,
    }));
    array_push_back(&jit.jcode, ((struct jit_code){
        .is_label_value = 1,
        .name = name,
        .base = jit_addr,
    }));
}

static void jit_gen_data(const struct symbol *sym)
{
    int index = array_len(&jit.jcode) > 0 ? array_back(&jit.jcode).base : 0;
    array_push_back(&jit.labels, ((struct jit_label){
        .name = str_init(sym_name(sym)),
        .index = index,
    }));
    int base = jit_addr;
    if (is_float(sym->type)) {
        base += 4;
        array_push_back(&jit.jcode, ((struct jit_code){
            .is_float_value = 1,
            .name = str_init(sym_name(sym)),
            .addr = jit_addr,
            .base = base,
            .value.u = sym->value.constant.u & 0xFFFFFFFFu,
            .label_text = str_init(sym_name(sym)),
        }));
    } else if (is_double(sym->type)) {
        base += 8;
        array_push_back(&jit.jcode, ((struct jit_code){
            .is_double_value = 1,
            .name = str_init(sym_name(sym)),
            .addr = jit_addr,
            .base = base,
            .value.i = sym->value.constant.i,
            .label_text = str_init(sym_name(sym)),
        }));
    } else {
        // TODO: long double
    }
    jit_addr = base;
}

static void jit_gen_builtin_startup()
{
    /*
     *  This is just a following code.
     *      - The address of 'main' will be resolved later.
     *      - strlen is provided as a faster & a compact function because it is used very often.
     *      - memcpy is used by the compiler to copy string data into char array.
     *
     *  __startup:
     *      0:  E9 00 00 00 00          jmp     main
     *
     *  strlen:
     *      0:  48 83 c8 ff             orq     $-1, %rax
     *  .LL1
     *      4:  48 ff c0                incq    %rax
     *      7:  80 3c 07 00             cmpb    $0, (%rdi,%rax,1)
     *      B:  75 F7                   jne     .LL1
     *      D:  c3                      ret
     *
     *  memcpy:
     *      0:  48 85 d2                testq  %rdx, %rdx
     *      3:  74 18                   je     .LL1
     *      5:  48 8d 0c 16             leaq   (%rsi,%rdx,1), %rcx
     *      9:  48 89 fa                movq   %rdi, %rdx
     *      C:  48 29 f2                subq   %rsi, %rdx
     *  .LL2:
     *      F:  0f b6 06                movzb  (%rsi), eax
     *     12:  88 04 32                movb   %al, (%dx,%rsi,1)
     *     15:  48 ff c6                incq   %rsi
     *     18:  48 39 ce                cmpq   %rcx, %rsi
     *     1B:  75 f2                   jne    .LL2
     *  .LL1:
     *     1D:  48 89 f8                movq   %rdi, %rax
     *     20:  c3                      ret
     */
    jit_gen_label(0, str_init("__startup"));
    jit_addr = 0;
    int base = 5;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = 0,
        .base = base,
        .code = (struct code){ .len = 5, .val = { 0xE9 } },
        .instr = (struct instruction){
                .opcode = INSTR_JMP,
                .optype = OPT_IMM,
                .source.imm.d.addr.label_name = str_init("main")
            },
    }));
    jit_addr = base;
    jit_gen_label(base, str_init("strlen"));
    base += 8;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 8, .val = { 0x48, 0x83, 0xC8, 0xFF, 0x48, 0xFF, 0xC0, 0x80 } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    base += 6;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 6, .val = { 0x3C, 0x07, 0x00, 0x75, 0xF7, 0xC3 } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    jit_gen_label(base, str_init("memcpy"));
    base += 8;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 8, .val = {  0x48, 0x85, 0xd2, 0x74, 0x18, 0x48, 0x8d, 0x0c } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    base += 8;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 8, .val = {  0x16, 0x48, 0x89, 0xfa, 0x48, 0x29, 0xf2, 0x0f } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    base += 8;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 8, .val = {  0xb6, 0x06, 0x88, 0x04, 0x32, 0x48, 0xff, 0xc6 } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    base += 8;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 8, .val = {  0x48, 0x39, 0xce, 0x75, 0xf2, 0x48, 0x89, 0xf8 } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
    base += 1;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = (struct code){ .len = 1, .val = {  0xc3 } },
        .instr = (struct instruction){
                .opcode = INSTR_BUILTIN,
            },
    }));
    jit_addr = base;
}

INTERNAL void jit_init(void)
{
    jit_setup_builtin();
    jit_gen_builtin_startup();
}

INTERNAL int jit_symbol(const struct symbol *sym)
{
    int base = jit_addr;
    int index = array_len(&jit.jcode) > 0 ? array_back(&jit.jcode).base : 0;
    String name = str_init(sym_name(sym));
    switch (sym->symtype) {
    case SYM_TENTATIVE: {
        assert(is_object(sym->type));
        array_push_back(&jit.labels, ((struct jit_label){
            .name = name,
            .index = index,
        }));
        base += size_of(sym->type);
        array_push_back(&jit.jcode, ((struct jit_code){
            .int_value_size = size_of(sym->type),
            .name = name,
            .addr = jit_addr,
            .base = base,
            .value.u = 0,
            .label_text = name,
        }));
        jit_addr = base;
        break;
    }
    case SYM_DEFINITION:
        if (!is_function(sym->type)) {
            jit_sym_curr = sym;
            break;
        }
        // fall through.
    case SYM_LABEL: {
        jit_gen_label(index, name);
        jit_sym_curr = NULL;
        break;
    }
    case SYM_STRING_VALUE: {
        base += sym->value.string.len + 1;
        array_push_back(&jit.labels, ((struct jit_label){
            .name = name,
            .index = index,
        }));
        array_push_back(&jit.jcode, ((struct jit_code){
            .is_string_value = 1,
            .name = name,
            .addr = jit_addr,
            .base = base,
            .label_text = sym->value.string,
        }));
        jit_addr = base;
        jit_sym_curr = NULL;
        break;
    }
    case SYM_CONSTANT: {
        jit_gen_data(sym);
        jit_sym_curr = NULL;
    }
    default:
        break;
    }
    return 0;
}

INTERNAL int jit_text(struct instruction instr)
{
    struct code c = encode(instr);
    int base = jit_addr + c.len;
    array_push_back(&jit.jcode, ((struct jit_code){
        .addr = jit_addr,
        .base = base,
        .code = c,
        .instr = instr,
    }));
    struct jit_code *jc = &array_back(&jit.jcode);
    switch (instr.optype) {
    case OPT_IMM:
        if (instr.source.imm.d.addr.label_name.len > 0) {
            jc->label_text = instr.source.imm.d.addr.label_name;
            jc->is_label_ref = 1;
        }
        break;
    case OPT_MEM:
    case OPT_MEM_REG:
        if (instr.source.mem.addr.label_name.len > 0) {
            jc->label_text = instr.source.mem.addr.label_name;
            jc->is_label_ref = 1;
        }
        break;
    case OPT_REG_MEM:
    case OPT_IMM_MEM:
        if (instr.dest.mem.addr.label_name.len > 0) {
            jc->label_text = instr.dest.mem.addr.label_name;
            jc->is_label_ref = 1;
        }
        break;
    }
    jit_addr = base;
    return 0;
}

INTERNAL int jit_data(struct immediate data)
{
    if (!jit_sym_curr) {
        return 0;
    }

    String name = str_init(sym_name(jit_sym_curr));
    int label_hidden = jit_sym_prev == jit_sym_curr;
    if (!label_hidden) {
        int index = array_len(&jit.jcode) > 0 ? array_back(&jit.jcode).base : 0;
        array_push_back(&jit.labels, ((struct jit_label){
            .name = name,
            .index = index,
        }));
    }
    int base = jit_addr;
    switch (data.type) {
    case IMM_INT: {
        base += data.w;
        switch (data.w) {
        case 1: {
            array_push_back(&jit.jcode, ((struct jit_code){
                .int_value_size = 1,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .value.u = data.d.byte,
                .label_text = name,
            }));
            break;
        }
        case 2: {
            array_push_back(&jit.jcode, ((struct jit_code){
                .int_value_size = 2,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .value.u = data.d.word,
                .label_text = name,
            }));
            break;
        }
        case 4: {
            array_push_back(&jit.jcode, ((struct jit_code){
                .int_value_size = 4,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .value.u = data.d.dword,
                .label_text = name,
            }));
            break;
        }
        default:
            assert(data.w == 8);
            array_push_back(&jit.jcode, ((struct jit_code){
                .int_value_size = 8,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .value.u = data.d.qword,
                .label_text = name,
            }));
        }
        break;
    }
    case IMM_ADDR: {
        assert(data.d.addr.sym);
        base += 8;
        array_push_back(&jit.jcode, ((struct jit_code){
            .is_address_value = 1,
            .is_label_ref = 1,
            .label_hidden = label_hidden,
            .name = name,
            .addr = jit_addr,
            .base = base,
            .value.u = data.d.addr.disp,
            .label_text = str_init(sym_name(data.d.addr.sym)),
        }));
        break;
    }
    case IMM_STRING: {
        if (data.w == data.d.string.len) {
            base += data.d.string.len;
            array_push_back(&jit.jcode, ((struct jit_code){
                .is_ascii_value = 1,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .label_text = data.d.string,
            }));
        } else {
            assert(data.w == data.d.string.len + 1);
            base += data.d.string.len + 1;
            array_push_back(&jit.jcode, ((struct jit_code){
                .is_string_value = 1,
                .label_hidden = label_hidden,
                .name = name,
                .addr = jit_addr,
                .base = base,
                .label_text = data.d.string,
            }));
        }
        break;
    }
    default:
        break;
    }

    jit_addr = base;
    jit_sym_prev = jit_sym_curr;
    return 0;
}

#define DEF_GET_REGISTER(name, op1, op2, op3, op4)\
    static uint64_t get_ ## name()\
    {\
        char code[] = { op1, op2, op3, op4 };\
        int size = sizeof(code) + 1;\
        uint8_t *buf;\
        jit_create((void**)&buf, size);\
        memcpy(buf, code, sizeof(code));\
        uint64_t r = jit_execute_uint64(buf);\
        jit_destroy(buf, size);\
        return r;\
    }\
    /**/

DEF_GET_REGISTER(rax, 0x48, 0x89, 0xc0, 0xC3);    // this is 'movq %rax,%rax'...
DEF_GET_REGISTER(rcx, 0x48, 0x89, 0xc8, 0xC3);
DEF_GET_REGISTER(rdx, 0x48, 0x89, 0xd0, 0xC3);
DEF_GET_REGISTER(rbx, 0x48, 0x89, 0xd8, 0xC3);
DEF_GET_REGISTER(rsp, 0x48, 0x89, 0xe0, 0xC3);
DEF_GET_REGISTER(rbp, 0x48, 0x89, 0xe8, 0xC3);
DEF_GET_REGISTER(rsi, 0x48, 0x89, 0xf0, 0xC3);
DEF_GET_REGISTER(rdi, 0x48, 0x89, 0xf8, 0xC3);
DEF_GET_REGISTER(r10, 0x4c, 0x89, 0xd0, 0xC3);
DEF_GET_REGISTER(r11, 0x4c, 0x89, 0xd8, 0xC3);
DEF_GET_REGISTER(r12, 0x4c, 0x89, 0xe0, 0xC3);
DEF_GET_REGISTER(r13, 0x4c, 0x89, 0xe8, 0xC3);
DEF_GET_REGISTER(r14, 0x4c, 0x89, 0xf0, 0xC3);
DEF_GET_REGISTER(r15, 0x4c, 0x89, 0xf8, 0xC3);

INTERNAL int jit_run(void)
{
    int main_found = jit_fix_code();
    // jit_print_code();
    // printf("%08p\n", jit.buffer);
    if (main_found) {
        // initialize
        void (*onstart)(void) = (void (*)(void))jit_get_builtin_function("__kcc_builtin_onstart");
        if (onstart) {
            onstart();
        }

        // run it.
        jit_return_value = jit_execute(jit.buffer);
        int laddr = jit_get_label_address("__kcc_call_atexit_funcs");
        if (laddr > 0) {
            jit_execute((char*)jit.buffer + laddr);
        }

        // finalize
        void (*onexit)(void) = (void (*)(void))jit_get_builtin_function("__kcc_builtin_onexit");
        if (onexit) {
            onexit();
        }
    }
    return 0;
}

INTERNAL int jit_print(void)
{
    jit_fix_code();
    jit_print_code();
    return 0;
}

INTERNAL int jit_finalize(void)
{
    jit_destroy(jit.buffer, jit.size);
    array_clear(&jit.labels);
    array_clear(&jit.jcode);
    if (jit_builtin_library) unload_library(jit_builtin_library);
    return 0;
}

INTERNAL int jit_get_return_value(void)
{
    return jit_return_value;
}
