#include <kcc/ext.h>
#include <stdlib.h>

static void *kccext_handle = NULL;

static void kcc_ext_atexit(void)
{
    if (kccext_handle) {
        __kcc_builtin_unloadlib(kccext_handle);
    }
}

void *kcc_extlib(void)
{
    if (!kccext_handle) {
        kccext_handle = __kcc_builtin_loadlib("kccext", NULL);
        atexit(kcc_ext_atexit);
    }
    return kccext_handle;
}
