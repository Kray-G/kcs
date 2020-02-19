#ifndef SETJMP_H
#define SETJMP_H

#include <stdint.h>

#if defined(__KCC_JIT__)
typedef struct __kcc_jmp_buf {      //  <offset>    <description>
    uint64_t rip;                   //  0x000       the return address from longjmp.
    uint64_t rsp;                   //  0x008       the stack pointer.
    uint64_t ret;                   //  0x010       the return address of setjmp.

    uint64_t rbx;                   //  0x018       saving the non-volatile registers.
    uint64_t rbp;                   //  0x020
    uint64_t r12;                   //  0x028
    uint64_t r13;                   //  0x030
    uint64_t r14;                   //  0x038
    uint64_t r15;                   //  0x040
} jmp_buf[1];

int64_t setjmp(jmp_buf buff);
int64_t longjmp(jmp_buf buff, int64_t ret);

#elif defined(__KCC__)

typedef struct __kcc_jmp_buf {
    int64_t ip;
    int64_t bp;
    int64_t sp;
} jmp_buf[1];

int64_t setjmp(jmp_buf buff);
int64_t longjmp(jmp_buf buff, int64_t ret);
#endif

#endif /* SETJMP_H */
