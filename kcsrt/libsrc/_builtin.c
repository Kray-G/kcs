#ifndef KCC_BUILTIN_C
#define KCC_BUILTIN_C

typedef void (*kcc_atexit_func_t)(void);
#define KCC_MAX_ATEXIT_FUNC_COUNT (128)
static kcc_atexit_func_t *kcc_atexit_func = 0;
static int kcc_atexit_func_count = 0;

int puts(const char *s);

int atexit(kcc_atexit_func_t func)
{
    if (!kcc_atexit_func) {
        kcc_atexit_func = __kcc_builtin_calloc(KCC_MAX_ATEXIT_FUNC_COUNT, sizeof(kcc_atexit_func_t));
        if (!kcc_atexit_func) {
            puts("memory allocation error on atexit.");
            return 1;
        }
    }
    if (kcc_atexit_func_count < KCC_MAX_ATEXIT_FUNC_COUNT) {
        kcc_atexit_func[kcc_atexit_func_count++] = func;
        return 0;
    }
    return 1;
}

void __kcc_call_atexit_funcs(void)
{
    if (kcc_atexit_func) {
        for (int i = kcc_atexit_func_count - 1; i >= 0; --i) {
            if (kcc_atexit_func[i]) {
                (*kcc_atexit_func[i])();
            }
        }
        __kcc_builtin_free(kcc_atexit_func);
        kcc_atexit_func = 0;
    }
}

#endif /* KCC_BUILTIN_C */
