#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include "../lib/timer.hpp"

/* ---------------------------------------------------------------------------------------------
    timer
--------------------------------------------------------------------------------------------- */

extern "C" {

DLLEXPORT void* timer_init(int argc, arg_type_t* argv)
{
    return new klib::timer();
}

DLLEXPORT double timer_elapsed(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0.0;
    }
    klib::timer* tmr = (klib::timer*)argv[0].value.p;
    return tmr->elapsed();
}

DLLEXPORT void timer_free(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    delete (klib::timer*)argv[0].value.p;
}

} // extern "C"
