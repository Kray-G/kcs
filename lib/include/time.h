#ifndef KCC_TIME_H
#define KCC_TIME_H

#include <_builtin.h>
#include <stdio.h>
#include <stdint.h>

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

typedef uint64_t time_t;

#define time(timer) __kcc_builtin_time(timer)
time_t mktime(struct tm *timeptr);
double difftime(time_t time2, time_t time1);
struct tm *localtime(const time_t *timer);
struct tm *localtime_r(const time_t *timer, struct tm *result);
struct tm *gmtime(const time_t *timer);
struct tm *gmtime_r(const time_t *timer, struct tm *result);
char *asctime(const struct tm *timeptr);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/time.c>
#endif
#endif

#endif /* KCC_TIME_H */
