#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include "lib/onig/src/oniguruma.h"

/* ---------------------------------------------------------------------------------------------
    initializer/finalizer
--------------------------------------------------------------------------------------------- */

DLLEXPORT int initialize(int argc, arg_type_t* argv)
{
    OnigEncoding use_encs[] = { ONIG_ENCODING_UTF8 };
    onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));
    return 0;
}

DLLEXPORT int finalize(int argc, arg_type_t* argv)
{
    onig_end();
    return 0;
}
