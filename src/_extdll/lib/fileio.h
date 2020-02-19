#ifndef KLIB_FILESYSTEM_FILEIO_H_
#define KLIB_FILESYSTEM_FILEIO_H_

#include <kcs/dllcore.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#if !defined(KLIB_CONFIG_FILEIO_WIN)

#ifdef _LARGEFILE_SOURCE
typedef fpos64_t klib_fpos_t;
#else
typedef fpos_t klib_fpos_t;
#endif

typedef FILE fileio;

#define klib_fopen          fopen
#define klib_fclose         fclose
#define klib_feof           feof
#define klib_fflush         fflush
#define klib_fgetc          fgetc
#define klib_fgets          fgets
#define klib_fprintf        fprintf
#define klib_fputc          fputc
#define klib_fputs          fputs
#define klib_fread          fread
#define klib_ftell          ftell
#define klib_fwrite         fwrite
#define klib_rewind         rewind
#define klib_ungetc         ungetc
#define klib_vfprintf       vfprintf
#define klib_vprintf        vprintf

#ifdef _LARGEFILE_SOURCE
#define klib_fgetpos        fgetpos64
#define klib_fsetpos        fsetpos64
#define klib_fseek          fseeko
#else
#define klib_fgetpos        fgetpos
#define klib_fsetpos        fsetpos
#define klib_fseek          fseek
#endif

#define fio_stdin           stdin
#define fio_stdout          stdout
#define fio_stderr          stderr

#else

typedef long long klib_fpos_t;

#define KLIB_FILEIO_BUFSIZE_SMALL   (512)
#define KLIB_FILEIO_BUFSIZE         (4096*16)
typedef struct fileio_ {
    HANDLE handle;
    int flags;
        // bit
        //  0:  0 for text, 1 for binary
        //  1:  input enable
        //  2:  output enable
        //  3:  EOF flag
        //  4:  ungetc enable
    int ungetc;
    char* buf;
    int len;
    int bufsize;
    int alcsize;
    bool allocated;
} fileio;

extern fileio* fio_stdio(int type);
extern int klib_flush_buffer(fileio* stream);
extern __int64 GetFilePointerEx(HANDLE handle);

extern int klib_fclose(fileio* stream);
extern int klib_feof(fileio* stream);
extern int klib_fflush(fileio* stream);
extern int klib_fgetc(fileio* stream);
extern int klib_fgetpos(fileio* stream, klib_fpos_t *pos);
extern char* klib_fgets(char* s, int n, fileio* stream);
extern fileio* klib_fopen(const char* filename, const char* mode);
extern int klib_fprintf(fileio* stream, const char* format, ...);
extern int klib_fputc(int c, fileio* stream);
extern int klib_fputs(const char* s, fileio* stream);
extern unsigned int klib_fread(void* ptr, unsigned int size, unsigned int nmemb, fileio* stream);
extern int klib_fseek(fileio* stream, int64_t offset, int origin);
extern int klib_fsetpos(fileio* stream, klib_fpos_t *pos);
extern int64_t klib_ftell(fileio* stream);
extern unsigned int klib_fwrite(const void* ptr, unsigned int size, unsigned int nmemb, fileio* stream);
extern void klib_rewind(fileio* stream);
extern int klib_ungetc(int c, fileio* stream);
extern int klib_vfprintf(fileio* stream, const char* format, va_list arg);
extern int klib_vprintf(const char* format, va_list arg);

#define fio_stdin   fio_stdio(0)
#define fio_stdout  fio_stdio(1)
#define fio_stderr  fio_stdio(2)

#endif

#endif  // KLIB_FILESYSTEM_FILEIO_H_
