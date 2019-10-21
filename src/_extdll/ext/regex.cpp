#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include <string.h>
#include "../lib/onig/src/oniguruma.h"

/* ---------------------------------------------------------------------------------------------
    regex
--------------------------------------------------------------------------------------------- */

extern "C" {

static char g_regex_last_error[ONIG_MAX_ERROR_MESSAGE_LEN] = {0};
typedef struct regex_pack_ {
    regex_t    *reg;
    int        start;
    OnigRegion *region;
} regex_pack_t;

DLLEXPORT void* regex_compile(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_STR) {
        strcpy(g_regex_last_error, "Invalid pattern.");
        return NULL;
    }

    regex_pack_t *rpack = new regex_pack_t();
    OnigErrorInfo einfo;
    const unsigned char *pattern = (const unsigned char *)argv[0].value.s;
    int r = onig_new(&rpack->reg, pattern, pattern + strlen((char* )pattern),
        ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) {
        onig_error_code_to_str((UChar* )g_regex_last_error, r, &einfo);
        return NULL;
    }
    rpack->start = 0;
    rpack->region = onig_region_new();

    return rpack;
}

DLLEXPORT const char *regex_last_error(int argc, arg_type_t* argv)
{
    return g_regex_last_error;
}

DLLEXPORT int regex_search(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_INT) {
        return 0;
    }

    regex_pack_t  *rpack = (regex_pack_t*)argv[0].value.p;
    unsigned char *str   = (unsigned char *)argv[1].value.s;
    int           pos    = (int)argv[2].value.i;
    int           len    = strlen((char*)str);
    int           index  = pos < 0 ? rpack->start : pos;
    if (index < 0 || len <= index) {
        rpack->start = 0;
        return 0;
    }

    onig_region_clear(rpack->region);
    unsigned char *end = str + len;
    int r = onig_search(rpack->reg, str, end, str + index, end, rpack->region, ONIG_OPTION_NONE);
    if (r == ONIG_MISMATCH) {
        return 0;
    }
    int searched = rpack->region->end[0];
    if (searched >= len) {
        rpack->start = -1;
    } else {
        rpack->start = searched;
    }

    return 1;
}

DLLEXPORT int regex_region_num_regs(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    return rpack->region->num_regs;
}

DLLEXPORT int regex_region_beg(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    int          index  = argv[1].value.i;
    return rpack->region->beg[index];
}

DLLEXPORT int regex_region_end(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    int          index  = argv[1].value.i;
    return rpack->region->end[index];
}

DLLEXPORT void regex_free(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    onig_region_free(rpack->region, 1);
    onig_free(rpack->reg);
    delete rpack;
}

} // extern "C"
