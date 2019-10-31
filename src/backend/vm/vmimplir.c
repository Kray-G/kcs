#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#include "vm.h"
#include "vminstr.h"

#include <stdio.h>

static int is_global = 0;
#define PUSH_CODE(vmcode) array_push_back(is_global ? &(glbl->code) : &(prog->code), vmcode);

static int vm_fgetc(FILE *fp)
{
    int ch = fgetc(fp);
    if (ch == EOF) {
        error("Import filed with unexpected file termination.\n");
        exit(1);
    }
    return ch;
}

static uint8_t vm_load_byte(FILE *fp)
{
    return vm_fgetc(fp) & 0xFF;
}

static uint16_t vm_load_word(FILE *fp)
{
    uint16_t r = (vm_fgetc(fp) & 0xFF) << 8;
    r |= vm_fgetc(fp) & 0xFF;
    return r;
}

static uint32_t vm_load_dword(FILE *fp)
{
    uint32_t r = ((uint32_t)vm_fgetc(fp) & 0xFF) << 24;
    r |= ((uint32_t)vm_fgetc(fp) & 0xFF) << 16;
    r |= ((uint32_t)vm_fgetc(fp) & 0xFF) << 8;
    r |= (uint32_t)vm_fgetc(fp) & 0xFF;
    return r;
}

static uint64_t vm_load_qword(FILE *fp)
{
    uint64_t r = ((uint64_t)vm_fgetc(fp) & 0xFF) << 56;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 48;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 40;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 32;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 24;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 16;
    r |= ((uint64_t)vm_fgetc(fp) & 0xFF) << 8;
    r |= (uint64_t)vm_fgetc(fp) & 0xFF;
    return r;
}

static uint8_t vm_load_type(FILE *fp)
{
    return vm_load_byte(fp);
}

static int vm_load_size(FILE *fp)
{
    return vm_load_dword(fp);
}

static int vm_load_address_index(FILE *fp)
{
    return vm_load_dword(fp);
}

static struct vm_address vm_load_address(FILE *fp, int global_base)
{
    struct vm_address addr;
    addr.name = str_init("-");
    addr.is_global = vm_load_byte(fp);
    addr.size = vm_load_dword(fp);
    addr.base = vm_load_dword(fp);
    addr.offset = vm_load_dword(fp);
    if (addr.is_global) {
        addr.base += global_base;
    }
    addr.index = addr.base + addr.offset;
    return addr;
}

static char *vm_load_label(char *buf, FILE *fp)
{
    int i = 0, ch;
    while ((ch = vm_fgetc(fp)) != 0) {
        buf[i++] = (char)ch;
    }
    buf[i] = 0;
    return buf;
}

static Type vm_load_cast_type(FILE *fp)
{
    Type type;
    uint8_t bitfield = vm_load_byte(fp);
    type.is_unsigned         = (bitfield & 0x80) == 0x80;
    type.is_const            = (bitfield & 0x40) == 0x40;
    type.is_volatile         = (bitfield & 0x20) == 0x20;
    type.is_restrict         = (bitfield & 0x10) == 0x10;
    type.is_pointer          = (bitfield & 0x08) == 0x08;
    type.is_pointer_const    = (bitfield & 0x04) == 0x04;
    type.is_pointer_volatile = (bitfield & 0x02) == 0x02;
    type.is_pointer_restrict = (bitfield & 0x01) == 0x01;
    type.type = vm_load_byte(fp);
    return type;
}

static void vm_load_module(struct vm_context *ctx, struct vm_program *prog, struct vm_program *glbl, FILE *fp, const char *file)
{
    char buf[1024] = {0};
    int global_base = get_global_variable_size();
    int global_added = 0;
    int opcode;
    while ((opcode = fgetc(fp)) != EOF) {
        switch (opcode) {
        case VM_NOP: {
            PUSH_CODE(((struct vm_code){
                .opcode = VM_NOP,
            }));
            continue;
        }
        case VM_LABEL: {
            vm_load_label(buf, fp);
            if (strcmp(buf, "KC$$")) {
                /* Not a magic number label. */
                if (buf[0] == '.') {
                    PUSH_CODE(((struct vm_code){
                        .opcode = VM_LABEL,
                        .type = VMOP_LABEL,
                        .d.lindex = -1,
                    }));
                } else {
                    if (!strcmp(buf, "_setup_global")) {
                        is_global = 1;
                    } else {
                        is_global = 0;
                        int index = array_len(&ctx->labels);
                        array_push_back(&ctx->labels, ((struct vm_label){ .index = -1, .name = str_init(buf) }));
                        PUSH_CODE(((struct vm_code){
                            .opcode = VM_LABEL,
                            .type = VMOP_LABEL,
                            .d.lindex = index,
                        }));
                    }
                }
            }
            continue;
        }
        case VM_HALT: {
            continue;
        }
        case VM_STORE:
        case VM_FZERO: {
            int type = vm_load_type(fp);
            struct vm_address addr = vm_load_address(fp, global_base);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .type = type,
                .d.addr = addr,
            }));
            continue;
        }
        case VM_CALL: {
            int type = vm_load_type(fp);
            if (type == VMOP_ADDR) {
                struct vm_address addr = vm_load_address(fp, global_base);
                PUSH_CODE(((struct vm_code){
                    .opcode = opcode,
                    .type = type,
                    .d.addr = addr,
                }));
            } else {
                vm_load_label(buf, fp);
                PUSH_CODE(((struct vm_code){
                    .opcode = opcode,
                    .type = (type == VMOP_FUNCADDR || type == VMOP_BUILTIN) ? VMOP_FUNCNAME : type,
                    .d.addr.name = str_init(buf),
                }));
            }
            continue;
        }
        case VM_RET:
        case VM_CLUP:
        case VM_ENTER: {
            int size = vm_load_size(fp);
            if (is_global) {
                global_added = size;
            } else {
                PUSH_CODE(((struct vm_code){
                    .opcode = opcode,
                    .d.size = size,
                }));
            }
            continue;
        }
        case VM_DEREF: {
            int type = vm_load_type(fp);
            struct vm_address addr = vm_load_address(fp, global_base);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .type = type,
                .d.addr = addr,
            }));
            continue;
        }
        case VM_CAST: {
            Type src = vm_load_cast_type(fp);
            Type dst = vm_load_cast_type(fp);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .d.cast.src = src,
                .d.cast.dst = dst,
            }));
            continue;
        }
        case VM_ALLOCA: {
            /* Not implemented, do it later. */
            continue;
        }
        case VM_JMP:
        case VM_JZ:
        case VM_JNZ: {
            int index = vm_load_address_index(fp);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .d.addr.index = index,
            }));
            continue;
        }
        case VM_PUSH: {
            struct vm_code code;
            uint8_t type = vm_load_type(fp);
            switch (type) {
            case VMOP_ADDR:
            case VMOP_VARI:
            case VMOP_VARU:
            case VMOP_VAROBJ:
            case VMOP_VARFL: {
                struct vm_address addr = vm_load_address(fp, global_base);
                code = ((struct vm_code){
                    .opcode = opcode,
                    .type = type,
                    .d.addr = addr,
                });
                break;
            }
            case VMOP_FUNCNAME: {
                vm_load_label(buf, fp);
                code = ((struct vm_code){
                    .opcode = VM_PUSH,
                    .type = VMOP_FUNCNAME,
                    .d.addr = (struct vm_address){
                        .name = str_init(buf),
                    },
                });
                break;
            }
            case VMOP_CHARP: {
                vm_load_label(buf, fp);
                int index = ctx->global_index;
                int len = strlen(buf) + 1;
                memcpy(prog->global + index, buf, len);
                ctx->global_index += len;
                union vm_imm imm = { .str = (char*)(prog->global + index) };
                code = ((struct vm_code){
                    .opcode = VM_PUSH,
                    .type = VMOP_CHARP,
                    .d.imm = imm,
                });
                break;
            }
            case VMOP_FLT: {
                vm_load_label(buf, fp);
                float f;
                sscanf(buf, "%f", &f);
                code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.f = f, };
                break;
            }
            case VMOP_DBL: {
                vm_load_label(buf, fp);
                double d;
                sscanf(buf, "%lf", &d);
                code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.d = d, };
                break;
            }
            case VMOP_LDBL: {
                vm_load_label(buf, fp);
                long double ld;
                sscanf(buf, "%llf", &ld);
                code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.ld = ld, };
                break;
            }
            case VMOP_INT8:     code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.byte  = vm_load_byte(fp),  }; break;
            case VMOP_INT16:    code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.word  = vm_load_word(fp),  }; break;
            case VMOP_INT32:    code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.dword = vm_load_dword(fp), }; break;
            case VMOP_INT64:    code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.qword = vm_load_qword(fp), }; break;
            case VMOP_UINT8:    code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.byte  = vm_load_byte(fp),  }; break;
            case VMOP_UINT16:   code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.word  = vm_load_word(fp),  }; break;
            case VMOP_UINT32:   code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.dword = vm_load_dword(fp), }; break;
            case VMOP_UINT64:   code = (struct vm_code){ .opcode = opcode, .type = type, .d.imm.qword = vm_load_qword(fp), }; break;
            default:
                error("Import filed with invalid module file: %s\n", file);
                exit(1);
            }
            PUSH_CODE(code);
            continue;
        }
        case VM_POP: {
            int type = vm_load_type(fp);
            if (type == VMOP_NONE) {
                PUSH_CODE(((struct vm_code){
                    .opcode = opcode,
                    .type = type,
                    .d.size = vm_load_size(fp),
                }));
            } else {
                struct vm_address addr = vm_load_address(fp, global_base);
                PUSH_CODE(((struct vm_code){
                    .opcode = opcode,
                    .type = type,
                    .d.addr = addr,
                }));
            }
            continue;
        }
        case VM_DEPOP: {
            int type = vm_load_type(fp);
            struct vm_address addr = vm_load_address(fp, global_base);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .type = type,
                .d.addr = addr,
            }));
            continue;
        }
        case VM_NOT:
        case VM_NEG:
        case VM_ADD:
        case VM_SUB:
        case VM_MUL:
        case VM_DIV:
        case VM_MOD:
        case VM_AND:
        case VM_OR:
        case VM_XOR:
        case VM_SHL:
        case VM_SHR:
        case VM_EQ:
        case VM_NE:
        case VM_GE:
        case VM_GT:
        case VM_LE:
        case VM_LT: {
            int type = vm_load_type(fp);
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
                .type = type,
            }));
            continue;
        }
        case VM_INC:
        case VM_DEC: {
            PUSH_CODE(((struct vm_code){
                .opcode = opcode,
            }));
            continue;
        }
        case VM_GLOBAL: {
            vm_load_label(buf, fp);
            array_push_back(&ctx->globals, ((struct vm_label){ .name = str_init(buf), .index = global_base }));
            continue;
        }
        case VM_REFLIB: {
            vm_load_label(buf, fp);
            add_import_module(buf); /* Load an additional module. */
            continue;
        }
        default:
            error("Import filed with invalid module file: %s\n", file);
            exit(1);
        }
    }
    add_global_variable_size(global_added);
}

INTERNAL void vm_import_module(struct vm_context *ctx, struct vm_program *prog, struct vm_program *glbl, String name)
{
    char file[256] = {0};
    #ifdef KCC_WINDOWS
    sprintf(file, "%s\\%s.lkx", get_exe_path(), str_raw(name));
    #else
    sprintf(file, "%s/%s.lkx", get_exe_path(), str_raw(name));
    #endif
    FILE *input = fopen(file, "rb");
    if (!input) {
        fprintf(stderr, "Could not open input file '%s'.\n", file);
        exit(1);
    }

    vm_load_module(ctx, prog, glbl, input, file);
}
