#include <kcc/dll.h>
#include <kcc/dllcore.h>

#include <stdio.h>
#include <stdint.h>
#include "lib/timer.hpp"
#include "lib/onig/src/oniguruma.h"
#include "lib/zip/miniz.h"
#include "lib/fileio.hpp"

/* ---------------------------------------------------------------------------------------------
    initializer/finalizer
--------------------------------------------------------------------------------------------- */

extern "C" {

DLLEXPORT int initialize(int argc, arg_type_t* argv)
{
    OnigEncoding use_encs[] = { ONIG_ENCODING_UTF8 };
    onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));
    return 0;
}

DLLEXPORT int finalize(int argc, arg_type_t* argv)
{
    onig_end();
    return 0;
}

} // extern "C"

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

/* ---------------------------------------------------------------------------------------------
    regex
--------------------------------------------------------------------------------------------- */

extern "C" {

static char g_regex_last_error[ONIG_MAX_ERROR_MESSAGE_LEN] = {0};
typedef struct regex_pack_ {
    regex_t    *reg;
    int        start;
    OnigRegion *region;
} regex_pack_t;

DLLEXPORT void* regex_compile(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_STR) {
        strcpy(g_regex_last_error, "Invalid pattern.");
        return NULL;
    }

    regex_pack_t *rpack = new regex_pack_t();
    OnigErrorInfo einfo;
    const unsigned char *pattern = (const unsigned char *)argv[0].value.s;
    int r = onig_new(&rpack->reg, pattern, pattern + strlen((char* )pattern),
        ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT, &einfo);
    if (r != ONIG_NORMAL) {
        onig_error_code_to_str((UChar* )g_regex_last_error, r, &einfo);
        return NULL;
    }
    rpack->start = 0;
    rpack->region = onig_region_new();

    return rpack;
}

DLLEXPORT const char *regex_last_error(int argc, arg_type_t* argv)
{
    return g_regex_last_error;
}

DLLEXPORT int regex_search(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_INT) {
        return 0;
    }

    regex_pack_t  *rpack = (regex_pack_t*)argv[0].value.p;
    unsigned char *str   = (unsigned char *)argv[1].value.s;
    int           pos    = (int)argv[2].value.i;
    int           len    = strlen((char*)str);
    int           index  = pos < 0 ? rpack->start : pos;
    if (index < 0 || len <= index) {
        rpack->start = 0;
        return 0;
    }

    onig_region_clear(rpack->region);
    unsigned char *end = str + len;
    int r = onig_search(rpack->reg, str, end, str + index, end, rpack->region, ONIG_OPTION_NONE);
    if (r == ONIG_MISMATCH) {
        return 0;
    }
    int searched = rpack->region->end[0];
    if (searched >= len) {
        rpack->start = -1;
    } else {
        rpack->start = searched;
    }

    return 1;
}

DLLEXPORT int regex_region_num_regs(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    return rpack->region->num_regs;
}

DLLEXPORT int regex_region_beg(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    int          index  = argv[1].value.i;
    return rpack->region->beg[index];
}

DLLEXPORT int regex_region_end(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT) {
        return 0;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    int          index  = argv[1].value.i;
    return rpack->region->end[index];
}

DLLEXPORT void regex_free(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    regex_pack_t *rpack = (regex_pack_t*)argv[0].value.p;
    onig_region_free(rpack->region, 1);
    onig_free(rpack->reg);
    delete rpack;
}

} // extern "C"

/* ---------------------------------------------------------------------------------------------
    zip/unzip
--------------------------------------------------------------------------------------------- */

extern "C" {

typedef struct mz_zip_archive_file_stat_info_ {
    mz_zip_archive *z;
    mz_zip_archive_file_stat s;
} mz_zip_archive_file_stat_info;

DLLEXPORT void* ziplib_open(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_STR || argv[1].type !=  C_INT) {
        return NULL;
    }
    mz_zip_archive* z = (mz_zip_archive*)calloc(1, sizeof(mz_zip_archive));
    switch ((char)argv[1].value.i) {
    case 'r':
        if (!mz_zip_reader_init_file(z, argv[0].value.s, 0)) {
            free(z);
            return NULL;
        }
        break;
    case 'w':
        if (!mz_zip_writer_init_file(z, argv[0].value.s, 0)) {
            free(z);
            return NULL;
        }
        break;
    case 'a':
        if (!mz_zip_reader_init_file(z, argv[0].value.s, 0)) {
            free(z);
            return NULL;
        }
        if (!mz_zip_writer_init_from_reader(z, argv[0].value.s)) {
            free(z);
            return NULL;
        }
        break;
    }
    return z;
}

DLLEXPORT int ziplib_finalize(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    if (!z) return 0;
    return mz_zip_writer_finalize_archive(z) ? 1 : 0;
}

DLLEXPORT void ziplib_close(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    if ((z->m_zip_mode == MZ_ZIP_MODE_WRITING) && (z->m_zip_mode != MZ_ZIP_MODE_WRITING_HAS_BEEN_FINALIZED)) {
        mz_zip_writer_finalize_archive(z);
    }
    mz_zip_end(z);
}

DLLEXPORT int ziplib_total_entries(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    if (!argv[0].value.p) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    return z->m_total_files;
}

DLLEXPORT void *ziplib_entry_open(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    const char *f = (const char *)argv[1].value.s;
    int index = mz_zip_reader_locate_file(z, f, NULL, 0);
    if (index >= 0) {
        mz_zip_archive_file_stat_info* e = (mz_zip_archive_file_stat_info*)calloc(1, sizeof(mz_zip_archive_file_stat_info));
        if (mz_zip_reader_file_stat(z, index, &e->s)) {
            e->z = z;
            return e;
        }
        free(e);
    }
    return NULL;
}

DLLEXPORT void *ziplib_entry_openbyindex(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_INT) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    int i = (int)argv[1].value.i;
    mz_zip_archive_file_stat_info* e = (mz_zip_archive_file_stat_info*)calloc(1, sizeof(mz_zip_archive_file_stat_info));
    if (mz_zip_reader_file_stat(z, i, &e->s)) {
        e->z = z;
        return e;
    }
    free(e);
    return NULL;
}

DLLEXPORT void ziplib_entry_close(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    free(e);
}

DLLEXPORT int ziplib_entry_index(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return -1;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return -1;
    return e->s.m_file_index;
}

DLLEXPORT uint64_t ziplib_entry_time(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return -1;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return -1;
    return (uint64_t)e->s.m_time;
}

DLLEXPORT const char *ziplib_entry_name(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return NULL;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return e->s.m_filename;
}

DLLEXPORT int ziplib_entry_isdir(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return -1;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return e->s.m_is_directory ? 1 : 0;
}

DLLEXPORT int ziplib_entry_isenc(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return -1;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return e->s.m_is_encrypted ? 1 : 0;
}

DLLEXPORT uint64_t ziplib_entry_size(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return e->s.m_uncomp_size;
}

DLLEXPORT uint64_t ziplib_entry_compsize(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return e->s.m_comp_size;
}

DLLEXPORT uint64_t ziplib_entry_crc32(int argc, arg_type_t* argv)
{
    if (argc != 1 || argv[0].type !=  C_PTR) {
        return 0;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    return (uint64_t)e->s.m_crc32;
}

DLLEXPORT int ziplib_entry_write(int argc, arg_type_t* argv)
{
    if (argc != 5 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || !(argv[2].type ==  C_PTR || argv[2].type ==  C_STR) || argv[3].type !=  C_INT || argv[4].type !=  C_INT) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    if (!z) return 0;
    const char *name = (const char *)argv[1].value.s;
    const void *buf = (argv[2].type ==  C_PTR) ? (const void *)argv[2].value.p : (const void *)argv[2].value.s;
    int bufsize = (int)argv[3].value.i;
    int level = (int)argv[4].value.i;
    return mz_zip_writer_add_mem(z, name, buf, bufsize, level) ? 1 : 0;
}

DLLEXPORT int ziplib_entry_write_from_file(int argc, arg_type_t* argv)
{
    if (argc != 4 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR || argv[2].type !=  C_STR || argv[3].type !=  C_INT) {
        return 0;
    }
    mz_zip_archive *z = (mz_zip_archive *)argv[0].value.p;
    if (!z) return 0;
    const char *name = (const char *)argv[1].value.s;
    const char *srcfile = (const char *)argv[2].value.s;
    int level = (int)argv[3].value.i;
    return mz_zip_writer_add_file(z, name, srcfile, NULL, 0, level) ? 1 : 0;
}

DLLEXPORT int ziplib_entry_read(int argc, arg_type_t* argv)
{
    if (argc != 3 || argv[0].type !=  C_PTR || !(argv[1].type ==  C_PTR || argv[1].type ==  C_STR) || argv[2].type !=  C_INT) {
        return 0;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    void *buf = (argv[1].type ==  C_PTR) ? (void *)argv[1].value.p : (void *)argv[1].value.s;
    int bufsize = (int)argv[2].value.i;
    return mz_zip_reader_extract_to_mem(e->z, e->s.m_file_index, buf, bufsize, 0) ? 1 : 0;
}

DLLEXPORT int ziplib_entry_read_to_file(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_STR) {
        return 0;
    }
    mz_zip_archive_file_stat_info *e = (mz_zip_archive_file_stat_info *)argv[0].value.p;
    if (!e) return 0;
    const char *name = (const char *)argv[1].value.s;
    return mz_zip_reader_extract_to_file(e->z, e->s.m_file_index, name, 0) ? 1: 0;
}

DLLEXPORT void ziplib_create(int argc, arg_type_t* argv)
{
    if (argc < 2) {
        return;
    }
    mz_zip_archive z[1] = {0};
    const char *name = (const char *)argv[0].value.s;
    int level = (int)argv[1].value.i;
    mz_zip_writer_init_file(z, name, 0);
    for (int i = 2; i < argc; ++i) {
        if (argv[i].type ==  C_STR) {
            const char *srcfile = (const char *)argv[i].value.s;
            if (!mz_zip_writer_add_file(z, name, srcfile, NULL, 0, level)) {
                return;
            }
        }
    }
    mz_zip_writer_finalize_archive(z);
    mz_zip_end(z);
}

} // extern "C"

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
    int64_t *pos = (int64_t *)argv[1].value.p;
    return klib::fgetpos(fp, pos);
}

DLLEXPORT int fileio_fsetpos(int argc, arg_type_t* argv)
{
    if (argc != 2 || argv[0].type !=  C_PTR || argv[1].type !=  C_PTR) {
        return 0;
    }
    klib::fileio *fp = (klib::fileio *)argv[0].value.p;
    int64_t *pos = (int64_t *)argv[1].value.p;
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
