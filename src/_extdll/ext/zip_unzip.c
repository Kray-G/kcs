#include <kcc/dll.h>
#include <kcc/dllcore.h>
#include "../lib/zip/miniz.h"

/* ---------------------------------------------------------------------------------------------
    zip/unzip
--------------------------------------------------------------------------------------------- */

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
            if (!mz_zip_writer_add_file(z, srcfile, srcfile, NULL, 0, level)) {
                return;
            }
        }
    }
    mz_zip_writer_finalize_archive(z);
    mz_zip_end(z);
}
