#ifndef KCC_TIME_TIME_C
#define KCC_TIME_TIME_C

#include <stdio.h>
#include <time.h>

time_t mktime(struct tm *tp)
{
    int year = tp->tm_year;
    int mon = tp->tm_mon;
    int day = tp->tm_mday;
    int hour = tp->tm_hour;
    int min = tp->tm_min;
    int sec = tp->tm_sec;
    if ((mon -= 2) <= 0) {
        mon += 12;
        year -= 1;
    }
    return ((((unsigned long)(year/4 - year/100 + year/400 + 367 * mon/12 + day) + year*365 - 719499) * 24 + hour) * 60 + min) * 60 + sec;
}

double difftime(time_t t1, time_t t0)
{
    return (double)t1 - (double)t0;
}

struct tm *localtime(const time_t *timer)
{
    static struct tm t;
    return localtime_r(timer, &t);
}

struct tm *localtime_r(const time_t *timer, struct tm *result)
{
    if (!result) {
        return NULL;
    }
    void *p = __kcc_builtin_gmtime_init(timer, 0);
    result->tm_sec   = __kcc_builtin_gmtime(p, 0);
    result->tm_min   = __kcc_builtin_gmtime(p, 1);
    result->tm_hour  = __kcc_builtin_gmtime(p, 2);
    result->tm_mday  = __kcc_builtin_gmtime(p, 3);
    result->tm_mon   = __kcc_builtin_gmtime(p, 4);
    result->tm_year  = __kcc_builtin_gmtime(p, 5);
    result->tm_wday  = __kcc_builtin_gmtime(p, 6);
    result->tm_yday  = __kcc_builtin_gmtime(p, 7);
    result->tm_isdst = __kcc_builtin_gmtime(p, 8);
    return result;
}

struct tm *gmtime(const time_t *timer)
{
    static struct tm t;
    return gmtime_r(timer, &t);
}

struct tm *gmtime_r(const time_t *timer, struct tm *result)
{
    if (!result) {
        return NULL;
    }
    void *p = __kcc_builtin_gmtime_init(timer, 1);
    result->tm_sec   = __kcc_builtin_gmtime(p, 0);
    result->tm_min   = __kcc_builtin_gmtime(p, 1);
    result->tm_hour  = __kcc_builtin_gmtime(p, 2);
    result->tm_mday  = __kcc_builtin_gmtime(p, 3);
    result->tm_mon   = __kcc_builtin_gmtime(p, 4);
    result->tm_year  = __kcc_builtin_gmtime(p, 5);
    result->tm_wday  = __kcc_builtin_gmtime(p, 6);
    result->tm_yday  = __kcc_builtin_gmtime(p, 7);
    result->tm_isdst = __kcc_builtin_gmtime(p, 8);
    return result;
}

char *asctime(const struct tm *tp)
{
    static const char wday_name[7][3] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char mon_name[12][3] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static char result[26];
    sprintf(
        result,
        "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
        wday_name[tp->tm_wday],
        mon_name[tp->tm_mon],
        tp->tm_mday, tp->tm_hour,
        tp->tm_min, tp->tm_sec,
        1900 + tp->tm_year
    );
    return result;
}

clock_t clock(void)
{
    return __kcc_builtin_clock();
}

#endif /* KCC_TIME_TIME_C */
