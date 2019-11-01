#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "vm.h"
#include "vminstr.h"
#include <lacc/array.h>
#include <lacc/context.h>

#define IDT4 "    "
#define IDT8 "        "
#define BASEP(addr) (addr.is_global ? "GP" : "BP")

static struct vm_program *vm_prog;

static const char *make_vm_type_string(int type)
{
    switch (type) {
    case VMOP_INT8:     return "i8";
    case VMOP_INT16:    return "i16";
    case VMOP_INT32:    return "i32";
    case VMOP_INT64:    return "i64";
    case VMOP_UINT8:    return "u8";
    case VMOP_UINT16:   return "u16";
    case VMOP_UINT32:   return "u32";
    case VMOP_UINT64:   return "u64";
    case VMOP_FLT:      return "fl32";
    case VMOP_DBL:      return "fl64";
    case VMOP_LDBL:     return "fl80";
    case VMOP_CHARP:    return "string";
    }
    return "(object)";
}

static void print_vm_call(struct vm_code *code)
{
    switch (code->type) {
    case VMOP_FUNCNAME:
        printf(IDT4 "%-24s%s\n", "call", str_raw(code->d.addr.name));
        break;
    case VMOP_FUNCADDR:
        printf(IDT4 "%-24s* %d <%s>\n", "call", code->d.addr.index, str_raw(code->d.addr.name));
        break;
    case VMOP_ADDR:
        printf(IDT4 "%-24s[%s%+d] : %s(%d)\n", "call", BASEP(code->d.addr), code->d.addr.index, str_raw(code->d.addr.name), code->d.addr.size);
        break;
    case VMOP_BUILTIN:
        printf(IDT4 "%-24s%s (built-in)\n", "call", str_raw(code->d.addr.name));
        break;
    default:
        printf(IDT4 "%-24s???\n", "call");
        break;
    }
}

static void print_vm_str(const char *opname, char *str)
{
    printf(IDT4 "%-24s&global + 0x%08x\n", opname, (int)(str - (char*)vm_prog->global));
}

static void print_vm_imm(const char *opname, enum vm_optype type, union vm_imm imm)
{
    switch (type) {
    case VMOP_INT8:     printf(IDT4 "%-24s%d (0x%x)\n", opname, (int8_t)imm.byte, (int8_t)imm.byte);         break;
    case VMOP_INT16:    printf(IDT4 "%-24s%d (0x%x)\n", opname, (int16_t)imm.word, (int16_t)imm.word);       break;
    case VMOP_INT32:    printf(IDT4 "%-24s%d (0x%x)\n", opname, (int32_t)imm.dword, (int32_t)imm.dword);     break;
    case VMOP_INT64:    printf(IDT4 "%-24s%lld (0x%llx)\n", opname, (int64_t)imm.qword, (int64_t)imm.qword); break;
    case VMOP_UINT8:    printf(IDT4 "%-24s%u (0x%x)\n", opname, imm.byte, imm.byte);                         break;
    case VMOP_UINT16:   printf(IDT4 "%-24s%u (0x%x)\n", opname, imm.word, imm.word);                         break;
    case VMOP_UINT32:   printf(IDT4 "%-24s%u (0x%x)\n", opname, imm.dword, imm.dword);                       break;
    case VMOP_UINT64:   printf(IDT4 "%-24s%llu (0x%llx)\n", opname, imm.qword, imm.qword);                   break;
    case VMOP_FLT:      printf(IDT4 "%-24s%e\n", opname, imm.f);                break;
    case VMOP_DBL:      printf(IDT4 "%-24s%e\n", opname, imm.d);                break;
    case VMOP_LDBL:     printf(IDT4 "%-24s%le\n", opname, imm.ld);              break;
    case VMOP_CHARP:    print_vm_str(opname, imm.str);                          break;
    }
}

static void print_vm_var(const char *opname, struct vm_address addr, int is_unsigned)
{
    if (addr.size > 8) {
        printf(IDT4 "%-24s[%s%+d] : %s(%d bytes)\n", opname, BASEP(addr),
            addr.index, str_raw(addr.name), addr.size);
    } else {
        printf(IDT4 "%-24s[%s%+d] : %s(%c%d)\n", opname, BASEP(addr),
            addr.index, str_raw(addr.name), is_unsigned ? 'u' : 'i', addr.size*8);
    }
}

static void print_vm_varobj(const char *opname, struct vm_address addr)
{
    printf(IDT4 "%-24s[%s%+d] : %s(object:%d)\n", opname, BASEP(addr), addr.index, str_raw(addr.name), addr.size);
}

static void print_vm_varfl(const char *opname, struct vm_address addr)
{
    printf(IDT4 "%-24s[%s%+d] : %s(fl%d)\n", opname, BASEP(addr), addr.index, str_raw(addr.name), addr.size*8);
}

static void print_vm_deref_var(const char *opname, struct vm_address addr)
{
    printf(IDT4 "%-24s[[%s%+d]] : *%s(%d)\n", opname, BASEP(addr), addr.index, str_raw(addr.name), addr.size);
}

static void print_vm_addr(const char *opname, struct vm_address addr)
{
    printf(IDT4 "%-24s%s%+d : &%s(%d)\n", opname, BASEP(addr), addr.index, str_raw(addr.name), addr.size);
}

static void print_vm_funcaddr(const char *opname, struct vm_address addr)
{
    printf(IDT4 "%-24s*%d : &%s(%d)\n", opname, addr.index, str_raw(addr.name), addr.size);
}

static void print_push(const char *opname, struct vm_code *code)
{
    switch (code->type) {
    case VMOP_VARI:
    case VMOP_VARU:
        print_vm_var(opname, code->d.addr, code->type == VMOP_VARU);
        break;
    case VMOP_VAROBJ:
        print_vm_varobj(opname, code->d.addr);
        break;
    case VMOP_VARFL:
        print_vm_varfl(opname, code->d.addr);
        break;
    case VMOP_ADDR:
        print_vm_addr(opname, code->d.addr);
        break;
    case VMOP_FUNCADDR:
        print_vm_funcaddr(opname, code->d.addr);
        break;
    case VMOP_BUILTIN:
        printf(IDT4 "%-24s%s (built-in)\n", opname, str_raw(code->d.addr.name));
        break;
    default:
        print_vm_imm(opname, code->type, code->d.imm);
        break;
    }
}

static void print_store_pop(const char *opname, struct vm_code *code)
{
    switch (code->type) {
    case VMOP_NONE:
        printf(IDT4 "%-24s(%d)\n", opname, code->d.size);
        break;
    case VMOP_VARI:
    case VMOP_VARU:
        print_vm_var(opname, code->d.addr, code->type == VMOP_VARU);
        break;
    case VMOP_VAROBJ:
        print_vm_varobj(opname, code->d.addr);
        break;
    default:
        printf(IDT4 "%-24s[%s%+d] : %s(%s)\n", opname, BASEP(code->d.addr),
                code->d.addr.index, str_raw(code->d.addr.name), make_vm_type_string(code->type));
        break;
    }
}

static void print_depop(const char *opname, struct vm_code *code)
{
    switch (code->type) {
    case VMOP_VARI:
    case VMOP_VARU:
        print_vm_deref_var(opname, code->d.addr);
        break;
    default:
        printf(IDT4 "%-24s[[%s%+d]%+d] : *(%s%+d)(%s)\n", opname, BASEP(code->d.addr),
                code->d.addr.base, code->d.addr.offset, str_raw(code->d.addr.name), code->d.addr.offset,
                make_vm_type_string(code->type));
        break;
    }
}

static void print_deref(struct vm_code *code)
{
    switch (code->type) {
    case VMOP_VARI:
    case VMOP_VARU:
        printf(IDT4 "%-24s(object(%d))\n", "deref", code->d.addr.size);
        break;
    default:
        printf(IDT4 "%-24s(%s)\n", "deref", make_vm_type_string(code->type));
        break;
    }
}

static void print_alloca(struct vm_code *code)
{
    printf(IDT4 "%-24s[BP%+d] : %s(%d)\n", "alloca", code->d.addr.index, str_raw(code->d.addr.name), code->d.addr.size);
}

static void print_jmp(struct vm_code *code)
{
    switch (code->opcode) {
    case VM_JMP:
        printf(IDT4 "%-24s* %+d <%s>\n", "jmp", code->d.addr.index, str_raw(code->d.addr.name));
        break;
    case VM_JZ:
        printf(IDT4 "%-24s* %+d <%s>\n", "jz",  code->d.addr.index, str_raw(code->d.addr.name));
        break;
    case VM_JNZ:
        printf(IDT4 "%-24s* %+d <%s>\n", "jnz", code->d.addr.index, str_raw(code->d.addr.name));
        break;
    }
}

static int print_jmptable_entry(struct vm_code *code)
{
    printf(IDT4 "%-24s* %+d <%s>\n", "  - entry", code->d.addr.index, str_raw(code->d.name));
}

static void print_type(Type type, int size)
{
    printf("(");
    if (is_const(type))                         printf("const ");
    if (is_volatile(type))                      printf("volatile ");
    if (is_restrict(type))                      printf("restrict ");
    if (is_unsigned(type) && !is_bool(type))    printf("unsigned ");
    switch (type_of(type)) {
    case T_VOID:        printf("void");                 break;
    case T_BOOL:        printf("_Bool");                break;
    case T_CHAR:        printf("char");                 break;
    case T_SHORT:       printf("short");                break;
    case T_INT:         printf("int");                  break;
    case T_LONG:        printf("long");                 break;
    case T_FLOAT:       printf("float");                break;
    case T_DOUBLE:      printf("double");               break;
    case T_LDOUBLE:     printf("long double");          break;
    case T_POINTER:     printf("*");                    break;
    case T_FUNCTION:    printf("<func>");               break;
    case T_ARRAY:       printf("array[%d]", size);      break;
    case T_STRUCT:      /* fall through. */
    case T_UNION:       printf("object{%d}", size);     break;
    }
    printf(")");
}

static void print_cast(struct vm_code *code)
{
    printf(IDT4 "%-24s", "cast");
    print_type(code->d.cast.src, size_of(code->d.cast.src));
    printf(" -> ");
    print_type(code->d.cast.dst, size_of(code->d.cast.dst));
    printf("\n");
}

static void print_vm_op(enum vm_opcode opcode, enum vm_optype type)
{
    const char *opname = "";
    switch (opcode) {
    case VM_NOT:    opname = "not"; break;
    case VM_NEG:    opname = "neg"; break;
    case VM_ADD:    opname = "add"; break;
    case VM_SUB:    opname = "sub"; break;
    case VM_MUL:    opname = "mul"; break;
    case VM_DIV:    opname = "div"; break;
    case VM_MOD:    opname = "mod"; break;
    case VM_AND:    opname = "and"; break;
    case VM_OR:     opname = "or";  break;
    case VM_XOR:    opname = "xor"; break;
    case VM_SHL:    opname = "shl"; break;
    case VM_SHR:    opname = "shr"; break;
    case VM_EQ:     opname = "eq";  break;
    case VM_NE:     opname = "ne";  break;
    case VM_GE:     opname = "ge";  break;
    case VM_GT:     opname = "gt";  break;
    case VM_LE:     opname = "le";  break;
    case VM_LT:     opname = "lt";  break;
    case VM_INC:    opname = "inc"; break;
    case VM_DEC:    opname = "dec"; break;
    }
    printf(IDT4 "%-24s(%s)\n", opname, make_vm_type_string(type));
}

static void print_label(struct vm_code *code)
{
    const char *name = get_vm_label_name(code->d.lindex);
    if (*name == '.') {
        printf("  %s\n", name);
    }
    else {
        printf("----------------------------------------------------------------\n%s\n", name);
    }
}

static const char *pickup_push_code(struct vm_code *code)
{
    switch (code->type) {
    case VMOP_UINT8:
    case VMOP_INT8:     return "push(8)";
    case VMOP_UINT16:
    case VMOP_INT16:    return "push(16)";
    case VMOP_UINT32:
    case VMOP_INT32:    return "push(32)";
    case VMOP_UINT64:
    case VMOP_INT64:    return "push(64)";
    }
    return  "push";
}

INTERNAL void print_vm_instruction(struct vm_program *prog, struct vm_code *code)
{
    vm_prog = prog;
    if (code->opcode == VM_LABEL) {
        print_label(code);
        return;
    }
    printf(IDT4 "%6d: ", code->index);
    switch (code->opcode) {
    case VM_NOP:    printf(IDT4 "%-24s\n", "nop");                              break;
    case VM_HALT:   printf(IDT4 "%-24s\n", "halt");                             break;
    case VM_STORE:  print_store_pop("store", code);                             break;
    case VM_FZERO:  print_vm_var("fill0", code->d.addr, 0);                     break;
    case VM_CALL:   print_vm_call(code);                                        break;
    case VM_CLUP:   printf(IDT4 "%-24s(%d)\n", "cleanup", code->d.size);        break;
    case VM_ENTER:  printf(IDT4 "%-24s%d\n", "enter", code->d.size);            break;
    case VM_RET:    printf(IDT4 "%-24s(%d)\n", "ret", code->d.size);            break;
    case VM_DEREF:  print_deref(code);                                          break;
    case VM_CAST:   print_cast(code);                                           break;
    case VM_ALLOCA: print_alloca(code);                                         break;
    case VM_JMP:    print_jmp(code);                                            break;
    case VM_JZ:     print_jmp(code);                                            break;
    case VM_JNZ:    print_jmp(code);                                            break;
    case VM_PUSH:   print_push(pickup_push_code(code), code);                   break;
    case VM_POP:    print_store_pop("pop", code);                               break;
    case VM_DEPOP:  print_depop("depop", code);                                 break;
    case VM_NOT:    print_vm_op(code->opcode, code->type);                      break;
    case VM_NEG:    print_vm_op(code->opcode, code->type);                      break;
    case VM_ADD:    print_vm_op(code->opcode, code->type);                      break;
    case VM_SUB:    print_vm_op(code->opcode, code->type);                      break;
    case VM_MUL:    print_vm_op(code->opcode, code->type);                      break;
    case VM_DIV:    print_vm_op(code->opcode, code->type);                      break;
    case VM_MOD:    print_vm_op(code->opcode, code->type);                      break;
    case VM_AND:    print_vm_op(code->opcode, code->type);                      break;
    case VM_OR:     print_vm_op(code->opcode, code->type);                      break;
    case VM_XOR:    print_vm_op(code->opcode, code->type);                      break;
    case VM_SHL:    print_vm_op(code->opcode, code->type);                      break;
    case VM_SHR:    print_vm_op(code->opcode, code->type);                      break;
    case VM_EQ:     print_vm_op(code->opcode, code->type);                      break;
    case VM_NE:     print_vm_op(code->opcode, code->type);                      break;
    case VM_GE:     print_vm_op(code->opcode, code->type);                      break;
    case VM_GT:     print_vm_op(code->opcode, code->type);                      break;
    case VM_LE:     print_vm_op(code->opcode, code->type);                      break;
    case VM_LT:     print_vm_op(code->opcode, code->type);                      break;
    case VM_INC:    print_vm_op(code->opcode, code->type);                      break;
    case VM_DEC:    print_vm_op(code->opcode, code->type);                      break;
    case VM_JMPTBL: printf(IDT4 "%-24s\n", "jmptbl");                           break;
    case VM_TBL_ENTRY: print_jmptable_entry(code);                              break;
    case VM_REFLIB: printf(IDT4 "%-24s%s\n", "#lib", str_raw(code->d.name));    break;
    case VM_SAVE_RETVAL: printf(IDT4 "%-24s\n", "save_retval");                 break;
    }
}

INTERNAL void print_vm_instruction_all(struct vm_program *prog)
{
    vm_prog = prog;
    int len = array_len(&(prog->code));
    for (int i = 0; i < len; ++i) {
        struct vm_code *code = &array_get(&(prog->code), i);
        print_vm_instruction(prog, code);
    }
}
