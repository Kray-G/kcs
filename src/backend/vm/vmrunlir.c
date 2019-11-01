#include <kcc.h>
#if !defined(AMALGAMATION) || !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "vm.h"
#include "vminstr.h"
#include <lacc/array.h>
#include <kcc/assert.h>

static int vm_return_value = 0;

// #define KCC_VM_DEBUG 1
// #define KCC_VM_DEBUG 2
// #define KCC_VM_DEBUG 3
#ifdef KCC_VM_DEBUG
#define CHECK_POINT() check_point(prog, code, start, sp, bp, gp, stack)
#endif
#ifndef CHECK_POINT
#define CHECK_POINT()
#endif

#define SIZE_OF_VM_STACK            (16*1024*1024)

#define NULLCHK(addr) {\
    if ((void*)addr < (void*)0x1000 && (void*)0 <= (void*)addr) {\
        print_stack(stack, start, sp);\
        print_register(stack, sp, bp, gp);\
        error("Oops, null-pointer access.\n");\
        exit(1);\
    }\
}\
/**/

#define PUSHIT(type,value)          { STACK_TOPI() = (type)(value); sp += 8; }
#define PUSHI(value)                { STACK_TOPI() = (uint64_t)(value); sp += 8; }
#define PUSHDT(type,value)          { STACK_TOPD() = (type)(value); sp += 8; }
#define PUSHF(value)                { STACK_TOPDT(float) = (float)(value); sp += 8; }
#define PUSHD(value)                { STACK_TOPDT(double) = (double)(value); sp += 8; }
#define PUSHLD(value)               { STACK_TOPDT(long double) = (long double)(value); sp += 16; }
#define POPI(lvalue,type,mask)      { sp -= 8; *(type*)(&lvalue) = (type)(STACK_TOPIT(type) mask); }
#define POPD(lvalue,type)           { sp -= 8; *(type*)(&lvalue) = (type)(STACK_TOPDT(type)); }
#define POPLD(lvalue)               { sp -= 16; *(long double*)(&lvalue) = (long double)(STACK_TOPLD()); }
#define POPI32(lvalue)              POPI(lvalue,uint32_t, & 0xFFFFFFFF)
#define POPI64(lvalue)              POPI(lvalue,uint64_t,/* no mask*/)
#define POPA(lvalue)                { sp -= 8; *(uint8_t**)(&lvalue) = (uint8_t*)(STACK_TOPI()); }
#define TOPIV(type)                 ((STACK_TOPIT_OFFSET(type,-8)))
#define TOPI(type,mask)             ((STACK_TOPIT_OFFSET(type,-8) mask))
#define TOPD(type)                  ((STACK_TOPDT_OFFSET(type,-8)))
#define TOPLD()                     (*(long double*)(&STACK_TOPLD_OFFSET(-16)))
#define STOREI(lvalue,type,mask)    { *(type*)(&lvalue) = (type)((STACK_TOPI_OFFSET(-8)) mask); }
#define STORED(lvalue,type)         { *(type*)(&lvalue) = (STACK_TOPDT_OFFSET(type,-8)); }
#define STORELD(lvalue)             { *(long double*)(&lvalue) = (STACK_TOPLD_OFFSET(-16)); }
#define DEREFI(type)                { uint8_t *addr; POPA(addr); NULLCHK(addr); type v1 = *(type*)(addr); PUSHI(v1); }
#define DEREFF()                    { uint8_t *addr; POPA(addr); NULLCHK(addr); float v1 = *(float*)(addr); PUSHF(v1); }
#define DEREFD()                    { uint8_t *addr; POPA(addr); NULLCHK(addr); double v1 = *(double*)(addr); PUSHD(v1); }
#define DEREFLD()                   { uint8_t *addr; POPA(addr); NULLCHK(addr); long double v1 = *(long double*)(addr); PUSHLD(v1); }
#define NEGI(type,mask)             { TOPIV(uint64_t) = -((type)TOPI(type, mask)); }
#define NEGF()                      { TOPD(float) = -(TOPD(float)); }
#define NEGD()                      { TOPD(double) = -(TOPD(double)); }
#define NEGLD()                     { TOPLD() = -(TOPLD()); }
#define OP2I(op,type,mask)          { type v2; POPI(v2, type, mask); TOPIV(uint64_t) = ((type)TOPI(type, mask) op v2); }
#define OP2F(op)                    { float v2; POPD(v2, float); TOPD(float) = ((float)TOPD(float) op v2); }
#define OP2D(op)                    { double v2; POPD(v2, double); TOPD(double) = ((double)TOPD(double) op v2); }
#define OP2LD(op)                   { long double v2; POPLD(v2); TOPLD() = ((long double)TOPLD() op v2); }
#define OP2DC(op,type)              { type v2; POPD(v2, type); TOPIV(uint64_t) = ((type)TOPD(type) op v2); }
#define OP2LDC(op)                  { long double v2; POPLD(v2); TOPIV(uint64_t) = ((long double)TOPLD() op v2); }
#define OP2S(op,type,mask)          { type v1; int32_t v2; POPI(v2, int32_t, mask); POPI(v1, type, mask); PUSHI(v1 op v2); }

#define CAST_FROM(type) {\
    if (is_unsigned(dst)) {\
        switch (type_of(dst)) {\
        case T_BOOL:\
            STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(STACK_TOPI_OFFSET(-8) ? 1 : 0);\
            break;\
        case T_CHAR:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_SHORT:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint16_t,-8) = (uint16_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint16_t,-8) = (uint16_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_INT:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint32_t,-8) = (uint32_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint32_t,-8) = (uint32_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_LONG:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint64_t,-8) = (uint64_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint64_t,-8) = (uint64_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_FLOAT:   STACK_TOPDT_OFFSET(float,-8) = (float)STACK_TOPI_OFFSET(-8); break;\
        case T_DOUBLE:  STACK_TOPDT_OFFSET(double,-8) = (double)STACK_TOPI_OFFSET(-8); break;\
        case T_LDOUBLE: STACK_TOPLD_OFFSET(-8) = (long double)STACK_TOPI_OFFSET(-8);\
            sp += 8;\
            break;\
        }\
    }\
    else {\
        switch (type_of(dst)) {\
        case T_BOOL:\
            STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(STACK_TOPI_OFFSET(-8) ? 1 : 0);\
            break;\
        case T_CHAR:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint8_t,-8) = (uint8_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_SHORT:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint16_t,-8) = (uint16_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint16_t,-8) = (uint16_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_INT:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint32_t,-8) = (uint32_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint32_t,-8) = (uint32_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_LONG:\
            if (is_signed(src)) STACK_TOPIT_OFFSET(uint64_t,-8) = (uint64_t)(type)STACK_TOPI_OFFSET(-8);\
            else                STACK_TOPIT_OFFSET(uint64_t,-8) = (uint64_t)(unsigned type)STACK_TOPI_OFFSET(-8);\
            break;\
        case T_FLOAT:   STACK_TOPDT_OFFSET(float,-8) = (float)STACK_TOPI_OFFSET(-8); break;\
        case T_DOUBLE:  STACK_TOPDT_OFFSET(double,-8) = (double)STACK_TOPI_OFFSET(-8); break;\
        case T_LDOUBLE: STACK_TOPLD_OFFSET(-8) = (long double)STACK_TOPI_OFFSET(-8);\
            sp += 8;\
            break;\
        }\
    }\
}\
/**/

static void print_stack(uint8_t *stack, int64_t start, int64_t sp)
{
    int i, j;
    printf("Stack:\n");
    for (i = start; i < sp; i += 16) {
        printf("    [%08p] ", stack+i);
        for (j = 0; j < 8; ++j) {
            int addr = i + j;
            if (addr >= sp) break;
            printf(" %02X", stack[addr]);
        }
        printf("   ");
        for (j = 8; j < 16; ++j) {
            int addr = i + j;
            if (addr >= sp) break;
            printf(" %02X", stack[addr]);
        }
        printf("\n");
    }
    fflush(stdout);
}

static void print_register(uint8_t* stack, int64_t sp, int64_t bp, int64_t gp)
{
    printf("gp = %016p\n", stack+gp);
    printf("bp = %016p\n", stack+bp);
    printf("sp = %016p\n", stack+sp);
    fflush(stdout);
}

static void check_point(struct vm_program *prog, struct vm_code *code, int64_t start, int64_t sp, int64_t bp, int64_t gp, uint8_t* stack)
{
    print_vm_instruction(prog, code);
    #if defined(KCC_VM_DEBUG) && KCC_VM_DEBUG == 1
    print_stack(stack, start, sp);
    print_register(stack, sp, bp, gp);
    getchar();
    #endif
    #if defined(KCC_VM_DEBUG) && KCC_VM_DEBUG == 2
    print_stack(stack, start, sp);
    print_register(stack, sp, bp, gp);
    #endif
}

static int run_vm_by_lir(struct vm_program *prog, int64_t ip, uint8_t *stack, int64_t bp)
{
    int64_t retval = 0;
    int32_t retsize;
    int64_t start = bp;
    int64_t sp = bp;
    int64_t gp = bp + 8;

    KCCVM_DEFINE_DISPATCH_TABLE();
    struct vm_code** base = prog->exec.data;

    VM_START()

    VM_CASE_(VM_NOP): { ++ip; NEXT(); }
    VM_CASE_(VM_LABEL): { assert(0); }
    VM_CASE_(VM_HALT): { ip = -1; VM_GOTO_END(); }
    VM_CASE_(VM_STORE): {
        int64_t gbp = code->d.addr.is_global ? gp : bp;
        switch (code->type) {
        case VMOP_INT8:     STOREI(stack[gbp+code->d.addr.index], int8_t, & 0xFF);          break;
        case VMOP_INT16:    STOREI(stack[gbp+code->d.addr.index], int16_t, & 0xFFFF);       break;
        case VMOP_INT32:    STOREI(stack[gbp+code->d.addr.index], int32_t, & 0xFFFFFFFF);   break;
        case VMOP_INT64:    STOREI(stack[gbp+code->d.addr.index], int64_t, /* no mask */);  break;
        case VMOP_UINT8:    STOREI(stack[gbp+code->d.addr.index], uint8_t, & 0xFF);         break;
        case VMOP_UINT16:   STOREI(stack[gbp+code->d.addr.index], uint16_t, & 0xFFFF);      break;
        case VMOP_UINT32:   STOREI(stack[gbp+code->d.addr.index], uint32_t, & 0xFFFFFFFF);  break;
        case VMOP_UINT64:   STOREI(stack[gbp+code->d.addr.index], uint64_t, /* no mask */)  break;
        case VMOP_FLT:      STORED(stack[gbp+code->d.addr.index], float);                   break;
        case VMOP_DBL:      STORED(stack[gbp+code->d.addr.index], double);                  break;
        case VMOP_LDBL:     STORELD(stack[gbp+code->d.addr.index]);                         break;
        case VMOP_CHARP:    STOREI(stack[gbp+code->d.addr.index], char*, /* no mask */);    break;
        default: {
            int size = code->d.addr.size;
            memcpy((stack+gbp+code->d.addr.index), STACK_TOPA(), size);
            break;
        }}
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_FZERO): {
        int64_t gbp = code->d.addr.is_global ? gp : bp;
        int size = code->d.addr.size;
        if (size > 8) {
            memset((stack+gbp+code->d.addr.index), 0, size);
        }
        else {
            switch (size) {
            case 1: *(uint8_t*) (stack+gbp+code->d.addr.index) = 0;  break;
            case 2: *(uint16_t*)(stack+gbp+code->d.addr.index) = 0;  break;
            case 4: *(uint32_t*)(stack+gbp+code->d.addr.index) = 0;  break;
            case 8: *(uint64_t*)(stack+gbp+code->d.addr.index) = 0;  break;
            }
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_CALL): {
        switch (code->type) {
        case VMOP_FUNCADDR: {
            PUSHI(ip+1);
            ip = code->d.addr.index;
            break;
        }
        case VMOP_ADDR: {
            PUSHI(ip+1);
            ip = *(uint64_t*)(stack + (code->d.addr.is_global ? gp : bp) + code->d.addr.index);
            break;
        }
        case VMOP_BUILTIN: {
            vm_builtin_t func = code->d.addr.func;
            if (!func) {
                error("Oops, function(%s) is not available.\n", str_raw(code->d.addr.name));
                exit(1);
            }
            retsize = func(stack, sp);
            sp += retsize;
            ++ip;
            break;
        }
        case VMOP_FUNCNAME:
            error("Oops, function(%s) is not available.\n", str_raw(code->d.addr.name));
            exit(1);
            break;
        }
        NEXT();
    }
    VM_CASE_(VM_RET): {
        int32_t size = code->d.size;
        size = PAD8(size);
        int vp = sp - size;
        sp = bp;
        POPI64(bp);
        POPI64(ip);
        if (size > 8) {
            memmove(STACK_TOPA(), stack+vp, size);
        }
        else {
            STACK_TOPI() = *(uint64_t*)(stack+vp);
        }
        sp += size;
        retsize = size;
        NEXT();
    }
    VM_CASE_(VM_CLUP): {
        assert(code->d.size > 0);
        int vp = sp - retsize;
        sp = vp - code->d.size;
        if (retsize > 8) {
            memmove(stack+sp, stack+vp, retsize);
        }
        else {
            STACK_TOPI() = *(uint64_t*)(stack+vp);
        }
        sp += retsize;
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_ENTER): {
        PUSHI(bp);
        bp = sp;
        sp += code->d.size;
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_DEREF): {
        switch (code->type) {
        case VMOP_INT8:     DEREFI(int8_t  ); break;
        case VMOP_INT16:    DEREFI(int16_t ); break;
        case VMOP_INT32:    DEREFI(int32_t ); break;
        case VMOP_INT64:    DEREFI(int64_t ); break;
        case VMOP_UINT8:    DEREFI(uint8_t ); break;
        case VMOP_UINT16:   DEREFI(uint16_t); break;
        case VMOP_UINT32:   DEREFI(uint32_t); break;
        case VMOP_UINT64:   DEREFI(uint64_t); break;
        case VMOP_FLT:      DEREFF(        ); break;
        case VMOP_DBL:      DEREFD(        ); break;
        case VMOP_LDBL:     DEREFLD(       ); break;
        default: {
            int size = code->d.addr.size;
            uint8_t *addr;
            POPA(addr);
            memcpy(STACK_TOPA(), addr, size);
            sp += PAD8(size);
            break;
        }}
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_CAST): {
        Type src = code->d.cast.src;
        Type dst = code->d.cast.dst;
        switch (type_of(src)) {
        case T_BOOL:
        case T_CHAR:
            CAST_FROM(char);
            break;
        case T_SHORT:
            CAST_FROM(short);
            break;
        case T_INT:
            CAST_FROM(int);
            break;
        case T_LONG:
            CAST_FROM(long);
            break;
        case T_FLOAT:
            switch (type_of(dst)) {
            case T_BOOL:    /* fall through. */
            case T_CHAR:    STACK_TOPIT_OFFSET(uint8_t,  -8) = (uint8_t) STACK_TOPDT_OFFSET(float, -8); break;
            case T_SHORT:   STACK_TOPIT_OFFSET(uint16_t, -8) = (uint16_t)STACK_TOPDT_OFFSET(float, -8); break;
            case T_INT:     STACK_TOPIT_OFFSET(uint32_t, -8) = (uint32_t)STACK_TOPDT_OFFSET(float, -8); break;
            case T_LONG:    STACK_TOPIT_OFFSET(uint64_t, -8) = (uint64_t)STACK_TOPDT_OFFSET(float, -8); break;
            case T_DOUBLE:  STACK_TOPDT_OFFSET(double, -8) = (double)STACK_TOPDT_OFFSET(float, -8);     break;
            case T_LDOUBLE: STACK_TOPLD_OFFSET(-8) = (long double)STACK_TOPD_OFFSET(-8);
                sp += 8;
                break;
            }
            break;
        case T_DOUBLE:
            switch (type_of(dst)) {
            case T_BOOL:    /* fall through. */
            case T_CHAR:    STACK_TOPIT_OFFSET(uint8_t,  -8) = (uint8_t) STACK_TOPD_OFFSET(-8);     break;
            case T_SHORT:   STACK_TOPIT_OFFSET(uint16_t, -8) = (uint16_t)STACK_TOPD_OFFSET(-8);     break;
            case T_INT:     STACK_TOPIT_OFFSET(uint32_t, -8) = (uint32_t)STACK_TOPD_OFFSET(-8);     break;
            case T_LONG:    STACK_TOPIT_OFFSET(uint64_t, -8) = (uint64_t)STACK_TOPD_OFFSET(-8);     break;
            case T_FLOAT:   STACK_TOPDT_OFFSET(float, -8) = (float)STACK_TOPD_OFFSET(-8);           break;
            case T_LDOUBLE: STACK_TOPLD_OFFSET(-8) = (long double)STACK_TOPD_OFFSET(-8);
                sp += 8;
                break;
            }
            break;
        case T_LDOUBLE:
            switch (type_of(dst)) {
            case T_BOOL:    /* fall through. */
            case T_CHAR:    STACK_TOPIT_OFFSET(uint8_t,  -16) = (uint8_t) STACK_TOPD_OFFSET(-16);   break;
            case T_SHORT:   STACK_TOPIT_OFFSET(uint16_t, -16) = (uint16_t)STACK_TOPD_OFFSET(-16);   break;
            case T_INT:     STACK_TOPIT_OFFSET(uint32_t, -16) = (uint32_t)STACK_TOPD_OFFSET(-16);   break;
            case T_LONG:    STACK_TOPIT_OFFSET(uint64_t, -16) = (uint64_t)STACK_TOPD_OFFSET(-16);   break;
            case T_FLOAT:   STACK_TOPDT_OFFSET(float, -16) = (float)STACK_TOPLD_OFFSET(-16);        break;
            case T_DOUBLE:  STACK_TOPDT_OFFSET(double, -16) = (double)STACK_TOPLD_OFFSET(-16);      break;
            }
            sp -= 8;
            break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_ALLOCA): {
        /* Not implemented, do it later. */
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_JMP): {
        ip += code->d.addr.index;
        NEXT();
    }
    VM_CASE_(VM_JZ): {
        uint64_t cond;
        POPI(cond, uint64_t, /* no mask */);
        if (cond == 0) ip += code->d.addr.index;
        else           ++ip;
        NEXT();
    }
    VM_CASE_(VM_JNZ): {
        uint64_t cond;
        POPI(cond, uint64_t, /* no mask */);
        if (cond != 0) ip += code->d.addr.index;
        else           ++ip;
        NEXT();
    }
    VM_CASE_(VM_PUSH): {
        int64_t gbp = code->d.addr.is_global ? gp : bp;
        switch (code->type) {
        case VMOP_VARI: {
            int addr = code->d.addr.index;
            int size = code->d.addr.size;
            switch (size) {
            case 1: PUSHI(*(int8_t*) (stack+gbp+addr)); break;
            case 2: PUSHI(*(int16_t*)(stack+gbp+addr)); break;
            case 4: PUSHI(*(int32_t*)(stack+gbp+addr)); break;
            case 8: PUSHI(*(int64_t*)(stack+gbp+addr)); break;
            default:
                memcpy(STACK_TOPA(), (stack+gbp+addr), size);
                sp += PAD8(size);
                break;
            }
            break;
        }
        case VMOP_VARU: {
            int addr = code->d.addr.index;
            int size = code->d.addr.size;
            switch (size) {
            case 1: PUSHI(*(uint8_t*) (stack+gbp+addr)); break;
            case 2: PUSHI(*(uint16_t*)(stack+gbp+addr)); break;
            case 4: PUSHI(*(uint32_t*)(stack+gbp+addr)); break;
            case 8: PUSHI(*(uint64_t*)(stack+gbp+addr)); break;
            default:
                memcpy(STACK_TOPA(), (stack+gbp+addr), size);
                sp += PAD8(size);
                break;
            }
            break;
        }
        case VMOP_VAROBJ: {
            int addr = code->d.addr.index;
            int size = code->d.addr.size;
            memcpy(STACK_TOPA(), (stack+gbp+addr), size);
            sp += PAD8(size);
            break;
        }
        case VMOP_VARFL: {
            int addr = code->d.addr.index;
            int size = code->d.addr.size;
            switch (size) {
            case 4:  PUSHF(*(float*)(stack+gbp+addr));           break;
            case 8:  PUSHD(*(double*)(stack+gbp+addr));          break;
            case 16: PUSHLD(*(long double*)(stack+gbp+addr));    break;
            }
            break;
        }
        case VMOP_INT8:     PUSHIT(int8_t,  code->d.imm.byte);   break;
        case VMOP_INT16:    PUSHIT(int16_t, code->d.imm.word);   break;
        case VMOP_INT32:    PUSHIT(int32_t, code->d.imm.dword);  break;
        case VMOP_INT64:    PUSHIT(int64_t, code->d.imm.qword);  break;
        case VMOP_UINT8:    PUSHIT(uint8_t,  code->d.imm.byte);  break;
        case VMOP_UINT16:   PUSHIT(uint16_t, code->d.imm.word);  break;
        case VMOP_UINT32:   PUSHIT(uint32_t, code->d.imm.dword); break;
        case VMOP_UINT64:   PUSHIT(uint64_t, code->d.imm.qword); break;
        case VMOP_FLT:      PUSHF(code->d.imm.f);                break;
        case VMOP_DBL:      PUSHD(code->d.imm.d);                break;
        case VMOP_LDBL:     PUSHLD(code->d.imm.ld);              break;
        case VMOP_CHARP:    PUSHI(code->d.imm.str);              break;
        case VMOP_ADDR:     PUSHI(stack+gbp+code->d.addr.index); break;
        case VMOP_FUNCADDR: PUSHI(code->d.addr.index);           break;
        case VMOP_BUILTIN:  PUSHI(code->d.addr.func);            break;
        default:
            break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_POP): {
        int64_t gbp = code->d.addr.is_global ? gp : bp;
        int addr = code->d.addr.index;
        switch (code->type) {
        case VMOP_NONE:     sp -= code->d.size;                             break;
        case VMOP_INT8:     POPI(stack[gbp+addr], int8_t, & 0xFF);          break;
        case VMOP_INT16:    POPI(stack[gbp+addr], int16_t, & 0xFFFF);       break;
        case VMOP_INT32:    POPI(stack[gbp+addr], int32_t, & 0xFFFFFFFF);   break;
        case VMOP_INT64:    POPI(stack[gbp+addr], int64_t, /* no mask */);  break;
        case VMOP_UINT8:    POPI(stack[gbp+addr], uint8_t, & 0xFF);         break;
        case VMOP_UINT16:   POPI(stack[gbp+addr], uint16_t, & 0xFFFF);      break;
        case VMOP_UINT32:   POPI(stack[gbp+addr], uint32_t, & 0xFFFFFFFF);  break;
        case VMOP_UINT64:   POPI(stack[gbp+addr], uint64_t, /* no mask */); break;
        case VMOP_FLT:      POPD(stack[gbp+addr], float);                   break;
        case VMOP_DBL:      POPD(stack[gbp+addr], double);                  break;
        case VMOP_LDBL:     POPLD(stack[gbp+addr]);                         break;
        case VMOP_CHARP:    POPI(stack[gbp+addr], char*, /* no mask */);    break;
        default: {
            int size = code->d.addr.size;
            sp -= PAD8(size);
            memcpy((stack+gbp+addr), STACK_TOPA(), size);
            break;
        }}
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_DEPOP): {
        int64_t gbp = code->d.addr.is_global ? gp : bp;
        uint8_t* addr = (uint8_t*)*(uint64_t*)(stack + gbp + code->d.addr.base);
        NULLCHK(addr);
        addr += code->d.addr.offset;
        switch (code->type) {
        case VMOP_INT8:     POPI(*(uint8_t**)addr, int8_t, & 0xFF);             break;
        case VMOP_INT16:    POPI(*(uint8_t**)addr, int16_t, & 0xFFFF);          break;
        case VMOP_INT32:    POPI(*(uint8_t**)addr, int32_t, & 0xFFFFFFFF);      break;
        case VMOP_INT64:    POPI(*(uint8_t**)addr, int64_t, /* no mask */);     break;
        case VMOP_UINT8:    POPI(*(uint8_t**)addr, uint8_t, & 0xFF);            break;
        case VMOP_UINT16:   POPI(*(uint8_t**)addr, uint16_t, & 0xFFFF);         break;
        case VMOP_UINT32:   POPI(*(uint8_t**)addr, uint32_t, & 0xFFFFFFFF);     break;
        case VMOP_UINT64:   POPI(*(uint8_t**)addr, uint64_t, /* no mask */);    break;
        case VMOP_FLT:      POPD(*(uint8_t**)addr, float);                      break;
        case VMOP_DBL:      POPD(*(uint8_t**)addr, double);                     break;
        case VMOP_LDBL:     POPLD(*(uint8_t**)addr);                            break;
        case VMOP_CHARP:    POPI(*(uint8_t**)addr, char*, /* no mask */);       break;
        default: {
            int size = code->d.addr.size;
            sp -= PAD8(size);
            memcpy(addr, STACK_TOPA(), size);
            break;
        }}
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_NOT): {
        switch (code->type) {
        case VMOP_INT8:     { int8_t val;   POPI(val, int8_t, & 0xFF);          PUSHI(~val); break; }
        case VMOP_INT16:    { int16_t val;  POPI(val, int16_t, & 0xFFFF);       PUSHI(~val); break; }
        case VMOP_INT32:    { int32_t val;  POPI(val, int32_t, & 0xFFFFFFFF);   PUSHI(~val); break; }
        case VMOP_INT64:    { int64_t val;  POPI(val, int64_t, /* no mask */);  PUSHI(~val); break; }
        case VMOP_UINT8:    { uint8_t val;  POPI(val, uint8_t, & 0xFF);         PUSHI(~val); break; }
        case VMOP_UINT16:   { uint16_t val; POPI(val, uint16_t, & 0xFFFF);      PUSHI(~val); break; }
        case VMOP_UINT32:   { uint32_t val; POPI(val, uint32_t, & 0xFFFFFFFF);  PUSHI(~val); break; }
        case VMOP_UINT64:   { uint64_t val; POPI(val, uint64_t, /* no mask */); PUSHI(~val); break; }
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_NEG): {
        switch (code->type) {
        case VMOP_INT8:     NEGI(int8_t,   & 0xFF);                 break;
        case VMOP_INT16:    NEGI(int16_t,  & 0xFFFF);               break;
        case VMOP_INT32:    NEGI(int32_t,  & 0xFFFFFFFF);           break;
        case VMOP_INT64:    NEGI(int64_t,  /* no mask */);          break;
        case VMOP_UINT8:    NEGI(uint8_t,  & 0xFF);                 break;
        case VMOP_UINT16:   NEGI(uint16_t, & 0xFFFF);               break;
        case VMOP_UINT32:   NEGI(uint32_t, & 0xFFFFFFFF);           break;
        case VMOP_UINT64:   NEGI(uint64_t, /* no mask */);          break;
        case VMOP_FLT:      NEGF();                                 break;
        case VMOP_DBL:      NEGD();                                 break;
        case VMOP_LDBL:     NEGLD();                                break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_ADD): {
        switch (code->type) {
        case VMOP_INT8:     OP2I( +, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I( +, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I( +, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I( +, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I( +, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I( +, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I( +, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I( +, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2F( +);                               break;
        case VMOP_DBL:      OP2D( +);                               break;
        case VMOP_LDBL:     OP2LD(+);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_SUB): {
        switch (code->type) {
        case VMOP_INT8:     OP2I( -, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I( -, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I( -, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I( -, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I( -, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I( -, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I( -, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I( -, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2F( -);                               break;
        case VMOP_DBL:      OP2D( -);                               break;
        case VMOP_LDBL:     OP2LD(-);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_MUL): {
        switch (code->type) {
        case VMOP_INT8:     OP2I( *, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I( *, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I( *, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I( *, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I( *, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I( *, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I( *, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I( *, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2F( *);                               break;
        case VMOP_DBL:      OP2D( *);                               break;
        case VMOP_LDBL:     OP2LD(*);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_DIV): {
        switch (code->type) {
        case VMOP_INT8:     OP2I( /, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I( /, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I( /, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I( /, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I( /, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I( /, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I( /, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I( /, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2F( /);                               break;
        case VMOP_DBL:      OP2D( /);                               break;
        case VMOP_LDBL:     OP2LD(/);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_MOD): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(%, int8_t,   & 0xFF);              break;
        case VMOP_INT16:    OP2I(%, int16_t,  & 0xFFFF);            break;
        case VMOP_INT32:    OP2I(%, int32_t,  & 0xFFFFFFFF);        break;
        case VMOP_INT64:    OP2I(%, int64_t,  /* no mask */);       break;
        case VMOP_UINT8:    OP2I(%, uint8_t,  & 0xFF);              break;
        case VMOP_UINT16:   OP2I(%, uint16_t, & 0xFFFF);            break;
        case VMOP_UINT32:   OP2I(%, uint32_t, & 0xFFFFFFFF);        break;
        case VMOP_UINT64:   OP2I(%, uint64_t, /* no mask */);       break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_AND): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(&, int8_t,   & 0xFF);              break;
        case VMOP_INT16:    OP2I(&, int16_t,  & 0xFFFF);            break;
        case VMOP_INT32:    OP2I(&, int32_t,  & 0xFFFFFFFF);        break;
        case VMOP_INT64:    OP2I(&, int64_t,  /* no mask */);       break;
        case VMOP_UINT8:    OP2I(&, uint8_t,  & 0xFF);              break;
        case VMOP_UINT16:   OP2I(&, uint16_t, & 0xFFFF);            break;
        case VMOP_UINT32:   OP2I(&, uint32_t, & 0xFFFFFFFF);        break;
        case VMOP_UINT64:   OP2I(&, uint64_t, /* no mask */);       break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_OR): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(|, int8_t,   & 0xFF);              break;
        case VMOP_INT16:    OP2I(|, int16_t,  & 0xFFFF);            break;
        case VMOP_INT32:    OP2I(|, int32_t,  & 0xFFFFFFFF);        break;
        case VMOP_INT64:    OP2I(|, int64_t,  /* no mask */);       break;
        case VMOP_UINT8:    OP2I(|, uint8_t,  & 0xFF);              break;
        case VMOP_UINT16:   OP2I(|, uint16_t, & 0xFFFF);            break;
        case VMOP_UINT32:   OP2I(|, uint32_t, & 0xFFFFFFFF);        break;
        case VMOP_UINT64:   OP2I(|, uint64_t, /* no mask */);       break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_XOR): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(^, int8_t,   & 0xFF);              break;
        case VMOP_INT16:    OP2I(^, int16_t,  & 0xFFFF);            break;
        case VMOP_INT32:    OP2I(^, int32_t,  & 0xFFFFFFFF);        break;
        case VMOP_INT64:    OP2I(^, int64_t,  /* no mask */);       break;
        case VMOP_UINT8:    OP2I(^, uint8_t,  & 0xFF);              break;
        case VMOP_UINT16:   OP2I(^, uint16_t, & 0xFFFF);            break;
        case VMOP_UINT32:   OP2I(^, uint32_t, & 0xFFFFFFFF);        break;
        case VMOP_UINT64:   OP2I(^, uint64_t, /* no mask */);       break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_SHL): {
        switch (code->type) {
        case VMOP_INT8:     OP2S(<<, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2S(<<, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2S(<<, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2S(<<, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2S(<<, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2S(<<, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2S(<<, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2S(<<, uint64_t, /* no mask */);      break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_SHR): {
        switch (code->type) {
        case VMOP_INT8:     OP2S(>>, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2S(>>, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2S(>>, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2S(>>, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2S(>>, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2S(>>, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2S(>>, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2S(>>, uint64_t, /* no mask */);      break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_EQ): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  ==, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I(  ==, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I(  ==, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I(  ==, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I(  ==, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I(  ==, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I(  ==, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I(  ==, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2DC( ==, float);                        break;
        case VMOP_DBL:      OP2DC( ==, double);                       break;
        case VMOP_LDBL:     OP2LDC(==);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_NE): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  !=, int8_t,   & 0xFF);             break;
        case VMOP_INT16:    OP2I(  !=, int16_t,  & 0xFFFF);           break;
        case VMOP_INT32:    OP2I(  !=, int32_t,  & 0xFFFFFFFF);       break;
        case VMOP_INT64:    OP2I(  !=, int64_t,  /* no mask */);      break;
        case VMOP_UINT8:    OP2I(  !=, uint8_t,  & 0xFF);             break;
        case VMOP_UINT16:   OP2I(  !=, uint16_t, & 0xFFFF);           break;
        case VMOP_UINT32:   OP2I(  !=, uint32_t, & 0xFFFFFFFF);       break;
        case VMOP_UINT64:   OP2I(  !=, uint64_t, /* no mask */);      break;
        case VMOP_FLT:      OP2DC( !=, float);                        break;
        case VMOP_DBL:      OP2DC( !=, double);                       break;
        case VMOP_LDBL:     OP2LDC(!=);                               break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_GE): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  >=, int8_t,   & 0xFF);           break;
        case VMOP_INT16:    OP2I(  >=, int16_t,  & 0xFFFF);         break;
        case VMOP_INT32:    OP2I(  >=, int32_t,  & 0xFFFFFFFF);     break;
        case VMOP_INT64:    OP2I(  >=, int64_t,  /* no mask */);    break;
        case VMOP_UINT8:    OP2I(  >=, uint8_t,  & 0xFF);           break;
        case VMOP_UINT16:   OP2I(  >=, uint16_t, & 0xFFFF);         break;
        case VMOP_UINT32:   OP2I(  >=, uint32_t, & 0xFFFFFFFF);     break;
        case VMOP_UINT64:   OP2I(  >=, uint64_t, /* no mask */);    break;
        case VMOP_FLT:      OP2DC( >=, float);                      break;
        case VMOP_DBL:      OP2DC( >=, double);                     break;
        case VMOP_LDBL:     OP2LDC(>=);                             break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_GT): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  >, int8_t,   & 0xFF);            break;
        case VMOP_INT16:    OP2I(  >, int16_t,  & 0xFFFF);          break;
        case VMOP_INT32:    OP2I(  >, int32_t,  & 0xFFFFFFFF);      break;
        case VMOP_INT64:    OP2I(  >, int64_t,  /* no mask */);     break;
        case VMOP_UINT8:    OP2I(  >, uint8_t,  & 0xFF);            break;
        case VMOP_UINT16:   OP2I(  >, uint16_t, & 0xFFFF);          break;
        case VMOP_UINT32:   OP2I(  >, uint32_t, & 0xFFFFFFFF);      break;
        case VMOP_UINT64:   OP2I(  >, uint64_t, /* no mask */);     break;
        case VMOP_FLT:      OP2DC( >, float);                       break;
        case VMOP_DBL:      OP2DC( >, double);                      break;
        case VMOP_LDBL:     OP2LDC(>);                              break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_LE): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  <=, int8_t,   & 0xFF);           break;
        case VMOP_INT16:    OP2I(  <=, int16_t,  & 0xFFFF);         break;
        case VMOP_INT32:    OP2I(  <=, int32_t,  & 0xFFFFFFFF);     break;
        case VMOP_INT64:    OP2I(  <=, int64_t,  /* no mask */);    break;
        case VMOP_UINT8:    OP2I(  <=, uint8_t,  & 0xFF);           break;
        case VMOP_UINT16:   OP2I(  <=, uint16_t, & 0xFFFF);         break;
        case VMOP_UINT32:   OP2I(  <=, uint32_t, & 0xFFFFFFFF);     break;
        case VMOP_UINT64:   OP2I(  <=, uint64_t, /* no mask */);    break;
        case VMOP_FLT:      OP2DC( <=, float);                      break;
        case VMOP_DBL:      OP2DC( <=, double);                     break;
        case VMOP_LDBL:     OP2LDC(<=);                             break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_LT): {
        switch (code->type) {
        case VMOP_INT8:     OP2I(  <, int8_t,   & 0xFF);            break;
        case VMOP_INT16:    OP2I(  <, int16_t,  & 0xFFFF);          break;
        case VMOP_INT32:    OP2I(  <, int32_t,  & 0xFFFFFFFF);      break;
        case VMOP_INT64:    OP2I(  <, int64_t,  /* no mask */);     break;
        case VMOP_UINT8:    OP2I(  <, uint8_t,  & 0xFF);            break;
        case VMOP_UINT16:   OP2I(  <, uint16_t, & 0xFFFF);          break;
        case VMOP_UINT32:   OP2I(  <, uint32_t, & 0xFFFFFFFF);      break;
        case VMOP_UINT64:   OP2I(  <, uint64_t, /* no mask */);     break;
        case VMOP_FLT:      OP2DC( <, float);                       break;
        case VMOP_DBL:      OP2DC( <, double);                      break;
        case VMOP_LDBL:     OP2LDC(<);                              break;
        }
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_INC): {
        ++TOPIV(int32_t);
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_DEC): {
        --TOPIV(int32_t);
        ++ip;
        NEXT();
    }
    VM_CASE_(VM_JMPTBL): {
        uint64_t index;
        POPI(index, uint32_t, /* no mask */);
        ip += index + 1;
        ip += base[ip]->d.addr.index;
        NEXT();
    }
    VM_CASE_(VM_SAVE_RETVAL): {
        retval = STACK_TOPI_OFFSET(-8);
        ++ip;
        NEXT();
    }
    VM_CASE_DEFAULT:
        assert(0);
    }

    VM_END();

    return retval;
}

INTERNAL int vm_get_return_value(void)
{
    return vm_return_value;
}

INTERNAL int vm_run_lir_impl(struct vm_program *prog, int entry, uint8_t *global, int gsize)
{
    /* stack is aligned by 8 bytes. */
    uint8_t* stack = calloc(SIZE_OF_VM_STACK + gsize + 1, sizeof(uint8_t));
    memcpy(stack, global, gsize);   /* global address is started from 0. */
    int stack_base = gsize + 1;
    stack_base = PAD_N(stack_base, 16);
    vm_return_value = run_vm_by_lir(prog, entry, stack, stack_base);
    free(stack);
    return 0;
}
