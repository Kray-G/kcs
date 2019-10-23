#include <kcc/ext.h>

void *timer_init(void)
{
    void *h = kcc_extlib();
    void *timer = __kcc_builtin_call_p(h, "timer_init");
    return timer;
}

double timer_elapsed(void *tmr)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(tmr);
    double elapsed = __kcc_builtin_call_d(h, "timer_elapsed");
    return elapsed;
}

void timer_free(void *tmr)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(tmr);
    __kcc_builtin_call(h, "timer_free");
}
