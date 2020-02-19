#ifndef KCC_STDIO_H
#define KCC_STDIO_H

#include <_builtin.h>
#include <stdarg.h>

#ifndef EOF
# define EOF (-1)
#endif

typedef void FILE;
typedef long int fpos_t;

extern FILE* _kcc_iobuf[3];
#define _KCC_IOBUFPTR(n)    (_kcc_iobuf[n] ? _kcc_iobuf[n] : (_kcc_iobuf[n] = get_std_filep(n)))
#define stdin   (_KCC_IOBUFPTR(0))
#define stdout  (_KCC_IOBUFPTR(1))
#define stderr  (_KCC_IOBUFPTR(2))

#define SEEK_SET (0)
#define SEEK_CUR (1)
#define SEEK_END (2)

#define putchar(c)  __kcc_builtin_putc(c)

int puts(const char *s);

int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf, int size, const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
int vsprintf(char *buf, const char *fmt, va_list ap);
int vsnprintf(char *buf, int size, const char *fmt, va_list ap);

int vsscanf(const char *src, const char *fmt, va_list ap);
int sscanf(const char *src, const char *fmt, ...);
int vfscanf(FILE *stream, const char *fmt, va_list ap);
int fcanf(FILE *stream, const char *fmt, ...);
int scanf(const char *fmt, ...);

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *stream);
int fgetpos(FILE *stream, int64_t *pos);
int fsetpos(FILE *stream, int64_t *pos);
int fflush(FILE *stream);
int feof(FILE *stream);
int fgetc(FILE *stream);
char *fgets(char *s, int n, FILE *stream);
int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int64_t ftell(FILE *stream);
void rewind(FILE *stream);
int ungetc(int c, FILE *stream);

int vfprintf(FILE* stream, const char *fmt, va_list ap);
int fprintf(FILE* stream, const char *fmt, ...);

#define getchar() fgetc(stdin)
#define getc(stream) fgetc(stream)
#ifdef KCC_USE_GETS
#define gets(s) fgets(s, 65535, stdin)
#else
#define gets(s) KCC_USE_GETS_for_gets
#endif

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/stdio.c>
#elif defined(__KCC__)
#pragma import("stdio");
#endif
#endif

#endif
