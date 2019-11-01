#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#include "vm.h"
#include "vminstr.h"

#include <kcc/assert.h>
#include <stdio.h>

static int sel_put_magic_number(FILE *fp)
{
    fputc(VM_LABEL, fp);
    fputs("KC$$", fp);
    fputc('\0', fp);
    return 1;
}

static int sel_put_global(FILE *fp, const char *name)
{
    fputc(VM_GLOBAL, fp);
    fputs(name, fp);
    fputc('\0', fp);
    return 1;
}

static int sel_put_code_byte(FILE *fp, uint8_t code)
{
    fputc(code, fp);
    return 1;
}

static int sel_put_code_word(FILE *fp, uint16_t code)
{
    fputc((code >> 8) & 0xFF, fp);
    fputc(code & 0xFF, fp);
    return 1;
}

static int sel_put_code_dword(FILE *fp, uint32_t code)
{
    fputc((code >> 24) & 0xFF, fp);
    fputc((code >> 16) & 0xFF, fp);
    fputc((code >> 8) & 0xFF, fp);
    fputc(code & 0xFF, fp);
    return 1;
}

static int sel_put_code_qword(FILE *fp, uint64_t code)
{
    fputc((code >> 56) & 0xFF, fp);
    fputc((code >> 48) & 0xFF, fp);
    fputc((code >> 40) & 0xFF, fp);
    fputc((code >> 32) & 0xFF, fp);
    fputc((code >> 24) & 0xFF, fp);
    fputc((code >> 16) & 0xFF, fp);
    fputc((code >> 8) & 0xFF, fp);
    fputc(code & 0xFF, fp);
    return 1;
}

static int sel_put_address_index(FILE *fp, const struct vm_address addr)
{
    sel_put_code_dword(fp, addr.index);
    return 1;
}

static int sel_put_address(FILE *fp, const struct vm_address addr)
{
    sel_put_code_byte(fp, addr.is_global ? 1 : 0);
    sel_put_code_dword(fp, addr.size);
    sel_put_code_dword(fp, addr.base);
    sel_put_code_dword(fp, addr.offset);
    if (addr.is_global) {
        sel_put_global(fp, str_raw(addr.name));
    }
    return 1;
}

static int sel_put_type(FILE *fp, const Type type)
{
    uint8_t bitfield = 0x00;
    if (type.is_unsigned)         bitfield |= 0x80;
    if (type.is_const)            bitfield |= 0x40;
    if (type.is_volatile)         bitfield |= 0x20;
    if (type.is_restrict)         bitfield |= 0x10;
    if (type.is_pointer)          bitfield |= 0x08;
    if (type.is_pointer_const)    bitfield |= 0x04;
    if (type.is_pointer_volatile) bitfield |= 0x02;
    if (type.is_pointer_restrict) bitfield |= 0x01;
    sel_put_code_byte(fp, bitfield);
    sel_put_code_byte(fp, type.type);
    return 1;
}

static int sel_put_string(FILE *fp, const char *str)
{
    fputs(str, fp);
    fputc('\0', fp);
    return 1;
}

static void serialize_lir(FILE *fp, struct vm_program *prog)
{
    sel_put_magic_number(fp);
    for (int ip = 0; ip < array_len(&(prog->code)); ++ip) {
        struct vm_code *code = &array_get(&(prog->code), ip);
        sel_put_code_byte(fp, code->opcode);
        switch (code->opcode) {
        case VM_NOP:
            continue;
        case VM_LABEL: {
            const char *label = get_vm_label_name(code->d.lindex);
            assert(label);
            if (label[0] == '.') {
                sel_put_string(fp, ".L$");
            } else {
                sel_put_string(fp, label);
            }
            continue;
        }
        case VM_HALT: {
            continue;
        }
        case VM_STORE:
        case VM_FZERO: {
            sel_put_code_byte(fp, code->type);
            sel_put_address(fp, code->d.addr);
            continue;
        }
        case VM_CALL: {
            sel_put_code_byte(fp, code->type);
            if (code->type == VMOP_ADDR) {
                sel_put_address(fp, code->d.addr);
            } else {
                sel_put_string(fp, str_raw(code->d.addr.name));
            }
            continue;
        }
        case VM_RET:
        case VM_CLUP:
        case VM_ENTER: {
            sel_put_code_dword(fp, code->d.size);
            continue;
        }
        case VM_DEREF: {
            sel_put_code_byte(fp, code->type);
            sel_put_address(fp, code->d.addr);
            continue;
        }
        case VM_CAST: {
            sel_put_type(fp, code->d.cast.src);
            sel_put_type(fp, code->d.cast.dst);
            continue;
        }
        case VM_ALLOCA: {
            /* Not implemented, do it later. */
            continue;
        }
        case VM_JMP:
        case VM_JZ:
        case VM_JNZ: {
            sel_put_address_index(fp, code->d.addr);
            continue;
        }
        case VM_PUSH: {
            sel_put_code_byte(fp, code->type);
            switch (code->type) {
            case VMOP_ADDR:
            case VMOP_VARI:
            case VMOP_VARU:
            case VMOP_VAROBJ:
            case VMOP_VARFL: {
                sel_put_address(fp, code->d.addr);
                break;
            }
            case VMOP_FUNCNAME: {
                sel_put_string(fp, str_raw(code->d.addr.name));
                break;
            }
            case VMOP_CHARP: {
                sel_put_string(fp, code->d.imm.str);
                break;
            }
            case VMOP_FLT: {
                char buf[32] = {0};
                snprintf(buf, 31, "%a", code->d.imm.f);
                sel_put_string(fp, buf);
                break;
            }
            case VMOP_DBL: {
                char buf[32] = {0};
                snprintf(buf, 31, "%a", code->d.imm.d);
                sel_put_string(fp, buf);
                break;
            }
            case VMOP_LDBL: {
                char buf[32] = {0};
                snprintf(buf, 31, "%lla", code->d.imm.ld);
                sel_put_string(fp, buf);
                break;
            }
            case VMOP_INT8:     sel_put_code_byte(fp, code->d.imm.byte);   break;
            case VMOP_INT16:    sel_put_code_word(fp, code->d.imm.word);   break;
            case VMOP_INT32:    sel_put_code_dword(fp, code->d.imm.dword); break;
            case VMOP_INT64:    sel_put_code_qword(fp, code->d.imm.qword); break;
            case VMOP_UINT8:    sel_put_code_byte(fp, code->d.imm.byte);   break;
            case VMOP_UINT16:   sel_put_code_word(fp, code->d.imm.word);   break;
            case VMOP_UINT32:   sel_put_code_dword(fp, code->d.imm.dword); break;
            case VMOP_UINT64:   sel_put_code_qword(fp, code->d.imm.qword); break;
            default:
                printf("type = %d\n", code->type);
                assert(0);
            }
            continue;
        }
        case VM_POP: {
            sel_put_code_byte(fp, code->type);
            if (code->type == VMOP_NONE) {
                sel_put_code_dword(fp, code->d.size);
            } else {
                sel_put_address(fp, code->d.addr);
            }
            continue;
        }
        case VM_DEPOP: {
            sel_put_code_byte(fp, code->type);
            sel_put_address(fp, code->d.addr);
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
            sel_put_code_byte(fp, code->type);
            continue;
        }
        case VM_INC:
        case VM_DEC:
        case VM_JMPTBL: {
            continue;
        }
        case VM_TBL_ENTRY: {
            sel_put_address_index(fp, code->d.addr);
            continue;
        }
        case VM_REFLIB: {
            sel_put_string(fp, str_raw(code->d.name));
            continue;
        }
        case VM_SAVE_RETVAL:
            continue;
        default:
            assert(0);
        }
    }
}

INTERNAL int vm_serialize_lir(FILE *fp, struct vm_program *prog)
{
    serialize_lir(fp, prog);
    return 0;
}
