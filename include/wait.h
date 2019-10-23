#ifndef SYS_WAIT_H_
#define SYS_WAIT_H_

#if !defined(_WIN32) && !defined(_WIN64)

#include <sys/wait.h>

#else

#define _W_INT(w)    (*(int *)&(w))    /* convert union wait to int */
#define WCOREFLAG    0200

#define _WSTATUS(x)             (_W_INT(x) & 0177)
#define _WSTOPPED               /* _WSTATUS if process is stopped */
#define WIFSTOPPED(x)           (_WSTATUS(x) == _WSTOPPED)
#define WSTOPSIG(x)             (_W_INT(x) >> 8)
#define WIFSIGNALED(x)          (_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
#define WTERMSIG(x)             (_WSTATUS(x))
#define WIFEXITED(x)            (_WSTATUS(x) == 0)
#define WEXITSTATUS(x)          (_W_INT(x) >> 8)
#define WCOREDUMP(x)            (_W_INT(x) & WCOREFLAG)
#define W_EXITCODE(ret, sig)    ((ret) << 8 | (sig))
#define w_sTOPCODE(sig)         ((sig) << 8 | _WSTOPPED)

#define WNOHANG                 (1)
#define WUNTRACED               (2)
#define WAIT_ANY                (-1)
#define WAIT_MYPGRP             (0)

#ifndef BYTE_ORDER
#define LITTLE_ENDIAN           (1)
#define BIG_ENDIAN              (2)
#define BYTE_ORDER              (LITTLE_ENDIAN)
#endif

union wait {
    int w_status;
    struct {
    #if BYTE_ORDER == LITTLE_ENDIAN
    uint32_t w_termsigf:7;
    uint32_t w_coredumpf:1;
    uint32_t w_retcodef:8;
    uint32_t w_fillerf:16;
    #endif
    #if BYTE_ORDER == BIG_ENDIAN
    uint32_t w_fillerf:16;
    uint32_t w_retcodef:8,
    uint32_t w_coredumpf:1;
    uint32_t w_termsigf:7;
    #endif
    } w_t;
    struct {
    #if BYTE_ORDER == LITTLE_ENDIAN
    uint32_t w_stopvalf:8;
    uint32_t w_stopsigf:8;
    uint32_t w_fillerf:16;
    #endif
    #if BYTE_ORDER == BIG_ENDIAN
    uint32_t w_fillerf:16;
    uint32_t w_stopsigf:8;
    uint32_t w_stopvalf:8;
    #endif
    } w_s;
};

#define w_termsigf       (w_t.w_termsigf)
#define w_coredumpf      (w_t.w_coredumpf)
#define w_retcodef       (w_t.w_retcodef)
#define w_stopvalf       (w_s.w_stopvalf)
#define w_stopsigf       (w_s.w_stopsigf)

#endif
#endif

