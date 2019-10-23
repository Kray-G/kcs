#include <kcc/dll.h>
#include <kcc/dllcore.h>

#include <stdlib.h>

#if defined(KLIB_USE_DEFAULT_TIMER)

#include <time.h>
#define KLIB_TMR_DEFINE_CLOCK_MEMBER()  clock_t m_start
#define KLIB_TMR_DECL_CURRENT_DATAVAR() clock_t cur
#define KLIB_TMR_SET_START_TIME(tmr)    (tmr)->m_start = clock()
#define KLIB_TMR_GET_EXECUTE_TIME(tmr)  (((double)cur-(double)((tmr)->m_start))/CLOCKS_PER_SEC)
#define KLIB_TMR_SET_CURRENT_TIME()     cur = clock()

#elif defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#define KLIB_TMR_DEFINE_CLOCK_MEMBER()  LARGE_INTEGER m_start, m_freq
#define KLIB_TMR_DECL_CURRENT_DATAVAR() LARGE_INTEGER cur
#define KLIB_TMR_SET_START_TIME(tmr)    QueryPerformanceFrequency(&((tmr)->m_freq)),QueryPerformanceCounter(&((tmr)->m_start))
#define KLIB_TMR_GET_EXECUTE_TIME(tmr)  (((double)cur.QuadPart-(double)((tmr)->m_start).QuadPart)/(double)((tmr)->m_freq).QuadPart)
#define KLIB_TMR_SET_CURRENT_TIME()     QueryPerformanceCounter(&cur)

#else

#include <sys/time.h>
#include <time.h>
#include <xunistd.h>

#if defined(KLIB_TMR_USE_CLOCK_GETTIME) && defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)

#define KLIB_TMR_DEFINE_CLOCK_MEMBER()  struct timespec m_start
#define KLIB_TMR_DECL_CURRENT_DATAVAR() struct timespec cur
#define KLIB_TMR_SET_START_TIME(tmr)    clock_gettime(CLOCK_MONOTONIC, &((tmr)->m_start))
#define KLIB_TMR_MAKE_TIME(x)           (((double)(x).tv_sec)+((double)(x).tv_nsec)/1000000000)
#define KLIB_TMR_GET_EXECUTE_TIME(tmr)  (KLIB_TMR_MAKE_TIME(cur)-KLIB_TMR_MAKE_TIME((tmr)->m_start))
#define KLIB_TMR_SET_CURRENT_TIME()     clock_gettime(CLOCK_MONOTONIC, &cur)

#else

#define KLIB_TMR_DEFINE_CLOCK_MEMBER()  struct timeval m_start
#define KLIB_TMR_DECL_CURRENT_DATAVAR() struct timeval cur
#define KLIB_TMR_SET_START_TIME(tmr)    gettimeofday(&((tmr)->m_start), NULL)
#define KLIB_TMR_MAKE_TIME(x)           ((double)(x).tv_sec+((double)((x).tv_usec)/1000000))
#define KLIB_TMR_GET_EXECUTE_TIME(tmr)  (KLIB_TMR_MAKE_TIME(cur)-KLIB_TMR_MAKE_TIME((tmr)->m_start))
#define KLIB_TMR_SET_CURRENT_TIME()     gettimeofday(&cur, NULL)

#endif

#endif

/* ---------------------------------------------------------------------------------------------
    timer
--------------------------------------------------------------------------------------------- */

typedef struct timer_ {
    KLIB_TMR_DEFINE_CLOCK_MEMBER();
} timer;

DLLEXPORT void* timer_init(int argc, arg_type_t* argv)
{
    timer *tmr = (timer *)calloc(1, sizeof(timer));
    KLIB_TMR_SET_START_TIME(tmr);
    return tmr;
}

DLLEXPORT void timer_restart(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    timer *tmr = (timer *)argv[0].value.p;
    KLIB_TMR_SET_START_TIME(tmr);
}

DLLEXPORT double timer_elapsed(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0.0;
    }
    timer *tmr = (timer *)argv[0].value.p;
    KLIB_TMR_DECL_CURRENT_DATAVAR();
    KLIB_TMR_SET_CURRENT_TIME();
    return (KLIB_TMR_GET_EXECUTE_TIME(tmr));
}

DLLEXPORT void timer_free(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    free((timer*)argv[0].value.p);
}
