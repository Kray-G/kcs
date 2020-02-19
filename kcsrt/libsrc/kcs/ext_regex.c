#include <kcs/regex.h>

regex_t *regex_compile(const char *pattern)
{
    void *h = kcc_extlib();
    regex_t *regex = calloc(1, sizeof(regex_t));
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(pattern);
    regex->h = __kcc_builtin_call_p(h, "regex_compile");
    regex->num_regs = 0;
    return regex;
}

int regex_search_from(regex_t *regex, const char *str, int start)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(regex->h);
    __kcc_builtin_add_arg_s(str);
    __kcc_builtin_add_arg_i(start);
    int r = __kcc_builtin_call_i(h, "regex_search");
    if (!r) {
        return 0;
    }

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(regex->h);
    int num_regs = __kcc_builtin_call_i(h, "regex_region_num_regs");
    if (num_regs > 10) {
        num_regs = 10;
    }

    regex->num_regs = num_regs;
    for (int i = 0; i < num_regs; ++i) {
        __kcc_builtin_reset_args();
        __kcc_builtin_add_arg_p(regex->h);
        __kcc_builtin_add_arg_i(i);
        regex->beg[i] = __kcc_builtin_call_i(h, "regex_region_beg");
        regex->end[i] = __kcc_builtin_call_i(h, "regex_region_end");
    }

    return 1;
}

int regex_search(regex_t *regex, const char *str)
{
    return regex_search_from(regex, str, -1);
}

void regex_free(regex_t *regex)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(regex->h);
    __kcc_builtin_call(h, "regex_free");
    free(regex);
}
