#ifndef KCC_TEST_H
#define KCC_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>

#if defined(__KCC__)
#pragma disable("warning:all")
#endif

static int g_debug = 0;
static const char* g_testname = NULL;
static int total = 0;
static int errors = 0;

#define DEBUG_DISP_STEP()   if (g_debug) printf("[%s]: %d\n", file, line)

void error(char* file, int line, ...)
{
    char msg[1024] = {0};

    va_list ap;
    va_start(ap, line);
    vsnprintf(msg, 1023, "%s", ap);
    va_end(ap);

    ++errors;
    printf("[%s:%d] %s\n", file, line, msg);
}

void force_fail(const char* file, int line, const char* msg)
{
    ++errors;
    printf("[%s:%d] %s\n", file, line, msg);
}

void error_int(char* file, int line, int expected, int actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: %d\n", expected);
    printf("    Actual:   %d\n", actual);
}

void assert_equal_int(char* file, int line, int a, int b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (a != b) {
        error_int(file, line, a, b, msg);
    }
}

void error_long(char* file, int line, long expected, long actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: %ld\n", expected);
    printf("    Actual:   %ld\n", actual);
}

void assert_equal_long(char* file, int line, long a, long b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (a != b) {
        error_long(file, line, a, b, msg);
    }
}

void error_long_long(char* file, int line, long long expected, long long actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: %lld\n", expected);
    printf("    Actual:   %lld\n", actual);
}

void assert_equal_long_long(char* file, int line, long long a, long long b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (a != b) {
        error_long_long(file, line, a, b, msg);
    }
}

void error_float(char* file, int line, float expected, float actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: %e\n", expected);
    printf("    Actual:   %e\n", actual);
}

void assert_equal_float(char* file, int line, float a, float b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (a < b && FLT_EPSILON <= (b - a)) {
        error_float(file, line, a, b, msg);
    }
    else if (b < a && FLT_EPSILON <= (a - b)) {
        error_float(file, line, a, b, msg);
    }
}

void error_double(char* file, int line, double expected, double actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: %e\n", expected);
    printf("    Actual:   %e\n", actual);
}

void assert_equal_double(char* file, int line, double a, double b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (a < b && DBL_EPSILON <= (b - a)) {
        error_double(file, line, a, b, msg);
    }
    else if (b < a && DBL_EPSILON <= (a - b)) {
        error_double(file, line, a, b, msg);
    }
}

void error_string(char* file, int line, const char* expected, const char* actual, const char* msg)
{
    ++errors;
    printf("\n[%s:%d]\n", file, line);
    printf("    Error:    %s\n", msg);
    printf("    Expected: '%s'\n", expected);
    printf("    Actual:   '%s'\n", actual);
}

void assert_equal_string(char* file, int line, const char* a, const char* b, const char* msg)
{
    DEBUG_DISP_STEP();
    ++total;
    if (strcmp(a, b) != 0) {
        error_string(file, line, a, b, msg);
    }
}

void print(const char* name)
{
    g_testname = name;
#if 0
    for (int i = 0; i < 60; ++i) printf("%c", '=');
    printf("\nStart test: [%s]\n", name);
    for (int i = 0; i < 60; ++i) printf("%c", '-');
    printf("\n");
#endif
}

inline void report()
{
    if (errors == 0) {
        printf("[ %-22s ]: Passed all (%4d tests)\n", g_testname, total);
    }
    else {
        printf("[ %-22s ]: %4d errors in %4d tests\n", g_testname, errors, total);
    }
#if 0
    printf("[%s] Test Report\n", g_testname ? g_testname : "---");
    printf("    Total test  count: %4d\n", total);
    printf("    Total error count: %4d\n", errors);
    for (int i = 0; i < 60; ++i) printf("%c", '-');
    printf("\n");
#endif
}

#define ASSERT_EQUAL_FL(f, l, type, a, b) assert_equal_ ## type(f, l, a, b, #a " != " #b)
#define ASSERT_EQUAL(type, a, b) assert_equal_ ## type(__FILE__, __LINE__, a, b, #a " != " #b)
// #define ASSERT_EQUAL_S(type, a, b) assert_equal_ ## type(__FILE__, __LINE__, a, b, a " != " b)

#define fail(msg)                       force_fail(__FILE__, __LINE__, msg)

#define expect_fl(f, l, a, b)           ASSERT_EQUAL_FL(f, l, int, a, b)
#define expectl_fl(f, l, a, b)          ASSERT_EQUAL_FL(f, l, long, a, b)
#define expectll_fl(f, l, a, b)         ASSERT_EQUAL_FL(f, l, long_long, a, b)
#define expectf_fl(f, l, a, b)          ASSERT_EQUAL_FL(f, l, float, a, b)
#define expectd_fl(f, l, a, b)          ASSERT_EQUAL_FL(f, l, double, a, b)
#define expect_string_fl(f, l, a, b)    ASSERT_EQUAL_FL(f, l, string, a, b)

#define expect(a, b)                    ASSERT_EQUAL(int, a, b)
#define expectl(a, b)                   ASSERT_EQUAL(long, a, b)
#define expectll(a, b)                  ASSERT_EQUAL(long_long, a, b)
#define expectf(a, b)                   ASSERT_EQUAL(float, a, b)
#define expectd(a, b)                   ASSERT_EQUAL(double, a, b)
#define expect_string(a, b)             ASSERT_EQUAL(string, a, b)

void testmain();

int main(int ac, char** av)
{
    // g_debug = 1;
    testmain();
    report();
    return 0;
}

#endif
