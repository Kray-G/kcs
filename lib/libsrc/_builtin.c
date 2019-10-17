#ifndef KCC_BUILTIN_C
#define KCC_BUILTIN_C

typedef void (*kcc_atexit_func_t)(void);
#define KCC_MAX_ATEXIT_FUNC_COUNT (128)
static kcc_atexit_func_t kcc_atexit_func[KCC_MAX_ATEXIT_FUNC_COUNT] = {0};
static int kcc_atexit_func_count = 0;

int atexit(kcc_atexit_func_t func)
{
    if (kcc_atexit_func_count < KCC_MAX_ATEXIT_FUNC_COUNT) {
        kcc_atexit_func[kcc_atexit_func_count++] = func;
        return 0;
    }
    return 1;
}

void __kcc_call_atexit_funcs(void)
{
    for (int i = kcc_atexit_func_count - 1; i >= 0; --i) {
        if (kcc_atexit_func[i]) {
            (*kcc_atexit_func[i])();
        }
    }
}

#endif /* KCC_BUILTIN_C */
