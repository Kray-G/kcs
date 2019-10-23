#ifndef KCC_STDIO_C
#define KCC_STDIO_C

#define KCC_NO_IMPORT
#include <stdio.h>
#include <stdarg.h>
#undef KCC_NO_IMPORT

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

FILE* _kcc_iobuf[3] = {0};

void *get_std_filep(int type)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_i(type);
    return __kcc_builtin_call_p(h, "fileio_get_iobuf");
}

int puts(const char *ss)
{
    char *base = __kcc_builtin_acpdup(ss);
    char *s = base;
    while (*s) {
        if (putchar(*s++) == EOF) {
            goto ERROR;
        }
    }
    if (putchar('\n') == EOF) {
        goto ERROR;
    }
    __kcc_builtin_acpfree(base);
    return s - base + 1;

ERROR:
    __kcc_builtin_acpfree(base);
    return EOF;
}

static int ext_printf_ld(FILE *stream, const char *fmt, long v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_i(v);
    return __kcc_builtin_call_i(h, "fileio_printf_ld");
}

static int ext_printf_d(FILE *stream, const char *fmt, int v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_i(v);
    return __kcc_builtin_call_i(h, "fileio_printf_d");
}

static int ext_printf_lf(FILE *stream, const char *fmt, long double v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_d(v);
    return __kcc_builtin_call_i(h, "fileio_printf_lf");
}

static int ext_printf_f(FILE *stream, const char *fmt, double v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_d(v);
    return __kcc_builtin_call_i(h, "fileio_printf_f");
}

static int ext_printf_p(FILE *stream, const char *fmt, void *v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_p(v);
    return __kcc_builtin_call_i(h, "fileio_printf_p");
}

static int ext_printf_s(FILE *stream, const char *fmt, char *v)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_s(fmt);
    __kcc_builtin_add_arg_s(v);
    return __kcc_builtin_call_i(h, "fileio_printf_s");
}

#define __KCC_PRINTF_BUFFER_SIZE (128)

int vfprintf(FILE *fp, const char *f, va_list ap)
{
    int conv = (!fp || fp == stdout || fp == stderr) ? 1 : 0;
    const char *base = conv ? (const char *)__kcc_builtin_acpdup(f) : f;
    const char *fmt = base;
    int oc = 0;
    char b[__KCC_PRINTF_BUFFER_SIZE] = {0};
    while (1) {
        LOOPTOP:
        if (!*fmt) break;
        if (*fmt != '%') {
            if (fp) {
                fputc(*fmt++, fp);
            } else {
                __kcc_builtin_putc(*fmt++);
            }
            ++oc;
            continue;
        }

        ++fmt;
        if (*fmt == '%') {
            if (fp) {
                fputc('%', fp);
            } else {
                __kcc_builtin_putc('%');
            }
            ++oc;
            ++fmt;
            continue;
        }

        b[0] = '%';
        for (int i = 1; i < __KCC_PRINTF_BUFFER_SIZE; ++i) {
            char prev = b[i-1];
            b[i] = *fmt;
            switch (*fmt++) {
            case 'c': case 'x': case 'X': case 'u': case 'o': case 'd': case 'i': {
                b[i+1] = 0;
                if (prev == 'l') oc += (fp ? ext_printf_ld(fp, b, va_arg(ap, long)) : __kcc_builtin_printf_ld(b, va_arg(ap, long)));
                else             oc += (fp ? ext_printf_d(fp, b, va_arg(ap, int)) : __kcc_builtin_printf_d(b, va_arg(ap, int)));
                goto LOOPTOP;
            }
            case 'A': case 'a': case 'E': case 'e': case 'G': case 'g': case 'F': case 'f': {
                b[i+1] = 0;
                if (prev == 'l') oc += (fp ? ext_printf_lf(fp, b, va_arg(ap, long double)) : __kcc_builtin_printf_lf(b, va_arg(ap, long double)));
                else             oc += (fp ? ext_printf_f(fp, b, va_arg(ap, double)) : __kcc_builtin_printf_f(b, va_arg(ap, double)));
                goto LOOPTOP;
            }
            case 'p': {
                b[i+1] = 0;
                oc += (fp ? ext_printf_p(fp, b, va_arg(ap, void*)) : __kcc_builtin_printf_p(b, va_arg(ap, void*)));
                goto LOOPTOP;
            }
            case 's': {
                b[i+1] = 0;
                oc += (fp ? ext_printf_s(fp, b, va_arg(ap, char*)) : __kcc_builtin_printf_s(b, va_arg(ap, char*)));
                goto LOOPTOP;
            }
            case '*': {
                i += sprintf(b+i, "%d", va_arg(ap, int)) - 1;
                break;
            }
            default:
                break;
            }
        }
    }
    if (conv) {
        __kcc_builtin_acpfree(base);
    }
    return oc;
}

int fprintf(FILE* stream, const char *f, ...)
{
    va_list ap;
    va_start(ap, f);
    int oc = vfprintf(stream, f, ap);
    va_end(ap);
    return oc;
}

int vprintf(const char *f, va_list ap)
{
    return vfprintf(0, f, ap);
}

int printf(const char *f, ...)
{
    va_list ap;
    va_start(ap, f);
    int oc = vprintf(f, ap);
    va_end(ap);
    return oc;
}

static int __kcc_copy_buf(char* b, int start, const char* s, size_t limit)
{
    int c = 0;
    b += start;
    if (limit == 0) {
        while (*s) {
            *b++ = *s++;
            ++c;
        }
    }
    else {
        for (int i = start; i < limit && *s; ++i) {
            *b++ = *s++;
            ++c;
        }
    }
    return c;
}

int vsnprintf(char* buf, int size, const char* f, va_list ap)
{
    const char *base = f;
    const char *fmt = base;
    int oc = 0;
    char s[__KCC_PRINTF_BUFFER_SIZE] = {0};
    char b[__KCC_PRINTF_BUFFER_SIZE] = {0};
    while (1) {
        LOOPTOP:
        if (!*fmt) break;
        if (size > 0 && (size-1) <= oc) {
            *(buf+oc) = '\0';
            return oc;
        }
        if (*fmt != '%') {
            *(buf+oc) = (*fmt++);
            ++oc;
            continue;
        }

        ++fmt;
        if (*fmt == '%') {
            *(buf+oc) = '%';
            ++oc;
            ++fmt;
            continue;
        }

        b[0] = '%';
        for (int i = 1; i < __KCC_PRINTF_BUFFER_SIZE; ++i) {
            char prev = b[i-1];
            b[i] = *fmt;
            switch (*fmt++) {
            case 'c': case 'x': case 'X': case 'u': case 'o': case 'd': case 'i': {
                b[i+1] = 0;
                if (prev == 'l') __kcc_builtin_sprintf_ld(b, va_arg(ap, long), s);
                else             __kcc_builtin_sprintf_d(b, va_arg(ap, int), s);
                oc += __kcc_copy_buf(buf, oc, s, size);
                goto LOOPTOP;
            }
            case 'A': case 'a': case 'E': case 'e': case 'G': case 'g': case 'F': case 'f': {
                b[i+1] = 0;
                if (prev == 'l') __kcc_builtin_sprintf_lf(b, va_arg(ap, long double), s);
                else             __kcc_builtin_sprintf_f(b, va_arg(ap, double), s);
                oc += __kcc_copy_buf(buf, oc, s, size);
                goto LOOPTOP;
            }
            case 'p': {
                b[i+1] = 0;
                __kcc_builtin_sprintf_p(b, va_arg(ap, void*), s);
                oc += __kcc_copy_buf(buf, oc, s, size);
                goto LOOPTOP;
            }
            case 's': {
                b[i+1] = 0;
                __kcc_builtin_sprintf_s(b, va_arg(ap, char*), s);
                oc += __kcc_copy_buf(buf, oc, s, size);
                goto LOOPTOP;
            }
            case '*': {
                i += sprintf(b+i, "%d", va_arg(ap, int)) - 1;
                break;
            }
            default:
                break;
            }
        }
    }
    *(buf+oc) = '\0';
    return oc;
}

int vsprintf(char* buf, const char* fmt, va_list ap)
{
    return vsnprintf(buf, 0, fmt, ap);
}

int snprintf(char* buf, int size, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return r;
}

int sprintf(char* buf, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsprintf(buf, fmt, ap);
    va_end(ap);
    return r;
}

#define KCC_IS_ENTER(ch) ((ch) == '\n' || (ch) == '\r')
#define KCC_IS_SPACE(ch) ((ch) == ' ' || (ch) == '\t')
#define KCC_IS_WS(ch)    (KCC_IS_SPACE(ch) || KCC_IS_ENTER(ch))
#define KCC_SSCANF_READ_BUF(buf, stream, sp, width)\
    do {\
        int i;\
        if (stream) {\
            int ch;\
            for (i = 0; (width == 0 || i < width); i++) {\
                ch = fgetc(stream);\
                if (ch == EOF || KCC_IS_WS(ch)) break;\
                buf[i] = ch;\
            }\
            if (stream == stdin) {\
                while (!KCC_IS_ENTER(ch)) {\
                    ch = fgetc(stream);\
                    if (ch == EOF || KCC_IS_ENTER(ch) || !KCC_IS_SPACE(ch)) {\
                        ungetc(ch, stream);\
                        break;\
                    }\
                }\
            } else {\
                while (!KCC_IS_WS(ch)) {\
                    ch = fgetc(stream);\
                    if (ch == EOF || !KCC_IS_WS(ch)) {\
                        ungetc(ch, stream);\
                        break;\
                    }\
                }\
            }\
            buf[i] = 0;\
        } else {\
            for (i = 0; (width == 0 || i < width) && *sp != '\0' && !KCC_IS_WS(*sp); i++) buf[i] = *sp++;\
            while (KCC_IS_WS(*sp)) sp++;\
            buf[i] = 0;\
        }\
    } while (0)\
    /**/
#define KCC_SSCANF_GETVAL_I(type, width, buf, stream, sp, base)\
    do {\
        KCC_SSCANF_READ_BUF(buf, stream, sp, width);\
        if (assign) {\
            char *endp = NULL;\
            switch (type) {\
            case KCC_SSCANF_TYPE_CHAR: {\
                char v = (char)strtol(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, char *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_SHORT: {\
                short v = (short)strtol(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, short *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_LONGLONG: {\
                long long v = (long long)strtoll(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, long long *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_LONG: /* fall through */\
            default: {\
                long v = (long)strtol(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, long *) = v;\
                break;\
            }}\
        }\
    } while (0)\
    /**/
#define KCC_SSCANF_GETVAL_U(type, width, buf, stream, sp, base)\
    do {\
        KCC_SSCANF_READ_BUF(buf, stream, sp, width);\
        if (assign) {\
            char *endp = NULL;\
            switch (type) {\
            case KCC_SSCANF_TYPE_CHAR: {\
                unsigned char v = (unsigned char)strtoul(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, unsigned char *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_SHORT: {\
                unsigned short v = (unsigned short)strtoul(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, unsigned short *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_LONGLONG: {\
                unsigned long long v = (unsigned long long)strtoull(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, unsigned long long *) = v;\
                break;\
            }\
            case KCC_SSCANF_TYPE_LONG: /* fall through */\
            default: {\
                unsigned long v = (unsigned long)strtoul(buf, &endp, base);\
                if (buf == endp) return EOF;\
                *va_arg(ap, unsigned long *) = v;\
                break;\
            }}\
        }\
    } while (0)\
    /**/
#define KCC_SSCANF_TYPE_CHAR        0x01
#define KCC_SSCANF_TYPE_SHORT       0x02
#define KCC_SSCANF_TYPE_LONG        0x03
#define KCC_SSCANF_TYPE_LONGLONG    0x04
#define KCC_SSCANF_MODE_FOUND       0x01
#define KCC_SSCANF_MODE_NOT         0x02

int scanf_core(FILE *stream, const char *src, const char *fmt, va_list ap)
{
    if (!stream && !src) {
        return 0;
    }

    int conv = 0, chsetmode = 0, assign, width, type;
    const char *fp, *sp = src;
    char buf[__KCC_PRINTF_BUFFER_SIZE] = {'\0'};
    char chset[256] = {0};

    for (const char *fp = fmt; *fp != '\0'; fp++) {
        if (*fp != '%' && *fp != '\0') {
            if (!KCC_IS_WS(*fp)) {
                int ch;
                if (stream) ch = fgetc(stream);
                else        ch = *sp++;
                if (ch != *fp) return EOF;
            } else {
                int ch;
                if (stream) ch = fgetc(stream);
                else        ch = *sp++;
                if (!KCC_IS_WS(ch)) {
                    if (stream) ungetc(ch, stream);
                    else        --sp;
                } else {
                    --fp;
                }
            }
            continue;
        }
        if (*fp == '\0') break;
        if (*fp == '%') {
            ++fp;
            if (*fp == '*') {
                assign = 0;
                ++fp;
            } else {
                assign = 1;
            }
            width = 0;
            type = 0;
            chsetmode = 0;
        AGAIN:
            if (chsetmode > 0) {
                if (*fp == ']') {
                    char *rp = va_arg(ap, char *);
                    do {
                        int ch = 0;
                        if (stream) ch = fgetc(stream);
                        else        ch = *sp++;
                        if ((chsetmode == KCC_SSCANF_MODE_FOUND && chset[ch]) || (chsetmode == KCC_SSCANF_MODE_NOT && !chset[ch])) {
                            if (assign) {
                                *rp++ = ch;
                            }
                        } else {
                            if (stream) ungetc(*buf, stream);
                            else        --sp;
                            break;
                        }
                    } while (1);
                    continue;
                } else if (*fp == '\0') {
                    return EOF;
                } else {
                    int ch = *fp++;
                    if (*fp != '-') {
                        chset[ch] = 1;
                    } else {
                        int ech = *++fp;
                        if (ech < ch) {
                            int t = ch; ch = ech; ech = t;
                        }
                        for (int i = ch; i <= ech; ++i) {
                            chset[i] = 1;
                        }
                        ++fp;
                    }
                    goto AGAIN;
                }
            } else switch (*fp) {
            case '[':
                memset(chset, 0x00, 256);
                ++fp;
                if (*fp == '^') {
                    chsetmode = KCC_SSCANF_MODE_NOT;
                } else {
                    chsetmode = KCC_SSCANF_MODE_FOUND;
                }
                goto AGAIN;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                width = width * 10 + *fp - '0';
                ++fp;
                goto AGAIN;
            case 'h':
                ++fp;
                if (*fp == 'h') {
                    type = KCC_SSCANF_TYPE_SHORT;
                    ++fp;
                } else {
                    type = KCC_SSCANF_TYPE_CHAR;
                }
                goto AGAIN;
            case 'l':
                ++fp;
                if (*fp == 'l') {
                    type = KCC_SSCANF_TYPE_LONGLONG;
                    ++fp;
                } else {
                    type = KCC_SSCANF_TYPE_LONG;
                }
                goto AGAIN;

            case '%':
                if (assign) {
                    *va_arg(ap, char *) = '%';
                }
                break;
            case 'c':
                KCC_SSCANF_READ_BUF(buf, stream, sp, 1);
                if (assign) {
                    *va_arg(ap, char *) = *buf;
                }
                break;
            case 'i':
                KCC_SSCANF_GETVAL_I(type, width, buf, stream, sp, 0);
                break;
            case 'd':
                KCC_SSCANF_GETVAL_I(type, width, buf, stream, sp, 10);
                break;
            case 'u':
                KCC_SSCANF_GETVAL_U(type, width, buf, stream, sp, 10);
                break;
            case 'o':
                KCC_SSCANF_GETVAL_I(type, width, buf, stream, sp, 8);
                break;
            case 'x': case 'X':
                KCC_SSCANF_GETVAL_I(type, width, buf, stream, sp, 16);
                break;
            case 'f': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A':
                KCC_SSCANF_READ_BUF(buf, stream, sp, width);
                if (assign) {
                    char *endp = NULL;
                    if (type == KCC_SSCANF_TYPE_LONG ||  type == KCC_SSCANF_TYPE_LONGLONG) {
                        double v = strtod(buf, &endp);
                        if (buf == endp) return EOF;
                        *va_arg(ap, double *) = v;
                    } else {
                        float v = strtof(buf, &endp);
                        *va_arg(ap, float *) = v;
                    }
                }
                break;
            case 's':
                KCC_SSCANF_READ_BUF(buf, stream, sp, width);
                if (assign) {
                    strncpy(va_arg(ap, char *), buf, strlen(buf)+1);
                }
                break;
            }
            if (assign) {
                conv++;
            }
        }
    }
    return conv;
}

int vsscanf(const char *src, const char *fmt, va_list ap)
{
    return scanf_core(0, src, fmt, ap);
}

int sscanf(const char *src, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsscanf(src, fmt, ap);
    va_end(ap);
    return r;
}

int vfscanf(FILE *stream, const char *fmt, va_list ap)
{
    return scanf_core(stream, 0, fmt, ap);
}

int fscanf(FILE *stream, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vfscanf(stream, fmt, ap);
    va_end(ap);
    return r;
}

int scanf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vfscanf(stdin, fmt, ap);
    va_end(ap);
    return r;
}

/* FILE I/O */

FILE *fopen(const char *filename, const char *mode)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(filename);
    __kcc_builtin_add_arg_s(mode);
    return (FILE *)__kcc_builtin_call_p(h, "fileio_fopen");
}

int fclose(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fclose");
}

int fgetpos(FILE *stream, int64_t *pos)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_p(pos);
    return __kcc_builtin_call_i(h, "fileio_fgetpos");
}

int fsetpos(FILE *stream, int64_t *pos)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_p(pos);
    return __kcc_builtin_call_i(h, "fileio_fsetpos");
}

int fflush(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fflush");
}

int feof(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_feof");
}

int fgetc(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fgetc");
}

char *fgets(char *s, int n, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(s);
    __kcc_builtin_add_arg_i(n);
    __kcc_builtin_add_arg_p(stream);
    return (char *)__kcc_builtin_call_p(h, "fileio_fgets");
}

int fputc(int c, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_i(c);
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fputc");
}

int fputs(const char *s, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(s);
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fputs");
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ptr);
    __kcc_builtin_add_arg_i(size);
    __kcc_builtin_add_arg_i(nmemb);
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fread");
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ptr);
    __kcc_builtin_add_arg_i(size);
    __kcc_builtin_add_arg_i(nmemb);
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_fwrite");
}

int fseek(FILE *stream, long offset, int whence)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_add_arg_i(offset);
    __kcc_builtin_add_arg_i(whence);
    return __kcc_builtin_call_i(h, "fileio_fseek");
}

int64_t ftell(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_ftell");
}

void rewind(FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stream);
    __kcc_builtin_call(h, "fileio_rewind");
}

int ungetc(int c, FILE *stream)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_i(c);
    __kcc_builtin_add_arg_p(stream);
    return __kcc_builtin_call_i(h, "fileio_ungetc");
}

#endif
