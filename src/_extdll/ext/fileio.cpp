#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include "../lib/fileio.hpp"

/* ---------------------------------------------------------------------------------------------
    FILE I/O
--------------------------------------------------------------------------------------------- */

extern "C" {

DLLEXPORT void* fileio_get_iobuf(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_INT) {
        return 0;
    }
    switch (argv[0].value.i) {
    case 0:
        return fio_stdin;
    case 1:
        return fio_stdout;
    case 2:
        return fio_stderr;
    }
    return 0;
}

DLLEXPORT void* fileio_fopen(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_STR || argv[1].type !=  C_STR) {
        return 0;
    }
    const char *filename = (const char *)argv[0].value.s;
    const char *mode = (const char *)argv[1].value.s;
    return klib::fopen(filename, mode);
}

DLLEXPORT int fileio_fclose(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return EOF;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return EOF;
    return klib::fclose(fp);
}

DLLEXPORT int fileio_fgetpos(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    klib::fpos_t *pos = (klib::fpos_t *)argv[1].value.p;
    return klib::fgetpos(fp, pos);
}

DLLEXPORT int fileio_fsetpos(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    klib::fpos_t *pos = (klib::fpos_t *)argv[1].value.p;
    return klib::fsetpos(fp, pos);
}

DLLEXPORT int fileio_fflush(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    return klib::fflush(fp);
}

DLLEXPORT int fileio_feof(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 1;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 1;
    return klib::feof(fp);
}

DLLEXPORT int fileio_fgetc(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    return klib::fgetc(fp);
}

DLLEXPORT char *fileio_fgets(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_STR || argv[1].type !=  C_INT || argv[2].type !=  C_PTR) {
        return 0;
    }
    char *s = (char *)argv[0].value.s;
    int n = (int)argv[1].value.i;
    klib::fileio *fp = (klib::fileio *)argv[2].value.p;
    if (!fp) return 0;
    return klib::fgets(s, n, fp);
}

DLLEXPORT int fileio_fputc(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_INT || argv[1].type !=  C_PTR) {
        return 0;
    }
    int c = (int)argv[0].value.i;
    klib::fileio *fp = (klib::fileio *)argv[1].value.p;
    if (!fp) return 0;
    return klib::fputc(c, fp);
}

DLLEXPORT int fileio_fputs(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_STR || argv[1].type !=  C_PTR) {
        return 0;
    }
    const char *s = (const char *)argv[0].value.s;
    klib::fileio *fp = (klib::fileio *)argv[1].value.p;
    if (!fp) return 0;
    return klib::fputs(s, fp);
}

DLLEXPORT unsigned int fileio_fread(int argc, arg_type_t* argv)
{
    if (argc != 4 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT || argv[2].type !=  C_INT || argv[3].type !=  C_PTR) {
        return 0;
    }
    void *ptr = (void *)argv[0].value.p;
    size_t size = (int)argv[1].value.i;
    size_t nmemb = (int)argv[2].value.i;
    klib::fileio *fp = (klib::fileio *)argv[3].value.p;
    if (!fp) return 0;
    return klib::fread(ptr, size, nmemb, fp);
}

DLLEXPORT unsigned int fileio_fwrite(int argc, arg_type_t* argv)
{
    if (argc != 4 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT || argv[2].type !=  C_INT || argv[3].type !=  C_PTR) {
        return 0;
    }
    const void *ptr = (const void *)argv[0].value.p;
    size_t size = (int)argv[1].value.i;
    size_t nmemb = (int)argv[2].value.i;
    klib::fileio *fp = (klib::fileio *)argv[3].value.p;
    if (!fp) return 0;
    return klib::fwrite(ptr, size, nmemb, fp);
}

DLLEXPORT int fileio_fseek(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT || argv[2].type !=  C_INT) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    int64_t offset = (int64_t)argv[1].value.i;
    int whence = (int)argv[2].value.i;
    return klib::fseek(fp, offset, whence);
}

DLLEXPORT int64_t fileio_ftell(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    return klib::ftell(fp);
}

DLLEXPORT void fileio_rewind(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return;
    klib::rewind(fp);
}

DLLEXPORT int fileio_ungetc(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_INT || argv[1].type !=  C_PTR) {
        return 0;
    }
    int c = (int)argv[0].value.i;
    klib::fileio *fp = (klib::fileio *)argv[1].value.p;
    if (!fp) return 0;
    return klib::ungetc(c, fp);
}

DLLEXPORT int fileio_printf_ld(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_INT) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    long v = (long)argv[2].value.i;
    return klib::fprintf(fp, fmt, v);
}

DLLEXPORT int fileio_printf_d(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_INT) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    int v = (int)argv[2].value.i;
    return klib::fprintf(fp, fmt, v);
}

DLLEXPORT int fileio_printf_lf(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_DBL) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    long double v = (long double)argv[2].value.d;
    return klib::fprintf(fp, fmt, v);
}

DLLEXPORT int fileio_printf_f(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_DBL) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    double v = (double)argv[2].value.d;
    return klib::fprintf(fp, fmt, v);
}

DLLEXPORT int fileio_printf_p(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    void *v = (void *)argv[2].value.p;
    return klib::fprintf(fp, fmt, v);
}

DLLEXPORT int fileio_printf_s(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_STR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    if (!fp) return 0;
    const char *fmt = (const char *)argv[1].value.s;
    char *v = (char *)argv[2].value.s;
    return klib::fprintf(fp, fmt, v);
}

} // extern "C"
