#include <kcc.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif

#include "jit_util.h"

#if defined(KCC_WINDOWS)
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#pragma comment(lib, "imagehlp.lib")
#else
#include <sys/mman.h>
#endif

INTERNAL void jit_create(void **buffer, int size)
{
    #if defined(KCC_WINDOWS)
    *buffer = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    #else
    *buffer = mmap(0, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (*buffer == (void*)-1) {
        *buffer = 0;
    }
    #endif
}

INTERNAL void jit_destroy(void *buffer, int size)
{
    if (!buffer) {
        return;
    }

    #if defined(KCC_WINDOWS)
    VirtualFree(buffer, 0, MEM_RELEASE);
    #else
    munmap(buffer, size);
    #endif
}

#if defined(KCC_WINDOWS)

INTERNAL void print_exception_info(struct _EXCEPTION_POINTERS *info, int code)
{
    PEXCEPTION_RECORD per = info->ExceptionRecord;
    PCONTEXT pctx = info->ContextRecord;

    printf("-- Exception Record --\n");
    printf("Exception Code: %08X\n", per->ExceptionCode);
    printf("Exception Address: %08p\n", per->ExceptionAddress);

    printf("-- Context --\n");

    printf("RIP %016llX\tRBP %016llX\n", pctx->Rip, pctx->Rbp);
    printf("RAX %016llX\tRBX %016llX\nRCX %016llX\tRDX %016llX\n", pctx->Rax, pctx->Rbx, pctx->Rcx, pctx->Rdx);

    printf("Xmm0 %016llX %016llX\n", pctx->Xmm0.High, pctx->Xmm0.Low);
    printf("Xmm1 %016llX %016llX\n", pctx->Xmm1.High, pctx->Xmm1.Low);

    if (per->ExceptionAddress > (void*)0x1000) {
        char *p = (char*)per->ExceptionAddress;
        for (int i = 0; i < 8; ++i) {
            printf("%02X, ", (*p++) & 0xFF);
        }
        printf("\n");
    }

    printf("Exception Code: %X\n", code);
    exit(1);
}

INTERNAL int jit_execute_int(void *main_pos)
{
    EXCEPTION_POINTERS* info;
    __try {
        return ((int(*)(void))main_pos)();
    } __except (info = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER) {
        print_exception_info(info, GetExceptionCode());
    }
    return 1;
}

INTERNAL uint64_t jit_execute_uint64(void *main_pos)
{
    EXCEPTION_POINTERS* info;
    __try {
        return ((uint64_t(*)(void))main_pos)();
    } __except (info = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER) {
        print_exception_info(info, GetExceptionCode());
    }
    return 1;
}

INTERNAL double jit_execute_double(void *main_pos)
{
    EXCEPTION_POINTERS* info;
    __try {
        return ((double(*)(void))main_pos)();
    } __except (info = GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER) {
        print_exception_info(info, GetExceptionCode());
    }
    return -1.0;
}

INTERNAL void print_stack(void)
{
    unsigned int   i;
    void           *stack[100];
    unsigned short frames;
    SYMBOL_INFO    *symbol;
    HANDLE         process;

    process = GetCurrentProcess();

    SymInitialize(process, NULL, TRUE);
    frames               = CaptureStackBackTrace(0, 100, stack, NULL);
    symbol               = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[ i ]), 0, symbol);
        printf("%d: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
    }

    free(symbol);
}

#else

INTERNAL int jit_execute_int(void *main_pos)
{
    return ((int(*)(void))main_pos)();
}

INTERNAL uint64_t jit_execute_uint64(void *main_pos)
{
    return ((uint64_t(*)(void))main_pos)();
}

INTERNAL double jit_execute_double(void *main_pos)
{
    return ((double(*)(void))main_pos)();
}

INTERNAL void print_stack(void)
{
}

#endif

