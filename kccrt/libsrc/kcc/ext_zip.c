
#include <kcc/zip.h>

zip_t *zip_open(const char *zipname, char mode)
{
    void *h = kcc_extlib();
    zip_t *zip = calloc(1, sizeof(zip_t));
    zip->mode = mode;
    zip->level = 6; // by default.

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(zipname);
    __kcc_builtin_add_arg_i(mode);
    zip->h = __kcc_builtin_call_p(h, "ziplib_open");

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    zip->total_entries = __kcc_builtin_call_i(h, "ziplib_total_entries");

    return zip;
}

zip_t *zip_compression_level(zip_t *zip, int level)
{
    if (zip) {
        zip->level = level;
    }
    return zip;
}

void zip_close(zip_t *zip)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    __kcc_builtin_call(h, "ziplib_close");
    free(zip);
}

zip_entry_t *zip_entry_open(zip_t *zip, const char *entryname)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    __kcc_builtin_add_arg_s(entryname);
    void *e = __kcc_builtin_call_p(h, "ziplib_entry_open");
    if (!e) {
        return NULL;
    }

    zip_entry_t *ent = calloc(1, sizeof(zip_entry_t));
    ent->h = e;
    if (zip->mode == 'r' || zip->mode == 'a') {
        __kcc_builtin_reset_args();
        __kcc_builtin_add_arg_p(ent->h);
        ent->name = __kcc_builtin_call_p(h, "ziplib_entry_name");
        ent->time = __kcc_builtin_call_i(h, "ziplib_entry_time");
        ent->index = __kcc_builtin_call_i(h, "ziplib_entry_index");
        ent->isdir = __kcc_builtin_call_i(h, "ziplib_entry_isdir");
        ent->isenc = __kcc_builtin_call_i(h, "ziplib_entry_isenc");
        ent->size = __kcc_builtin_call_i(h, "ziplib_entry_size");
        ent->compsize = __kcc_builtin_call_i(h, "ziplib_entry_compsize");
        ent->crc32 = __kcc_builtin_call_i(h, "ziplib_entry_crc32");
        if (!ent->name) {
            ent->name = "(noname)";
        }
    }
    return ent;
}

zip_entry_t *zip_entry_openbyindex(zip_t *zip, int index)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    __kcc_builtin_add_arg_i(index);
    void *e = __kcc_builtin_call_p(h, "ziplib_entry_openbyindex");
    if (!e) {
        return NULL;
    }

    zip_entry_t *ent = calloc(1, sizeof(zip_entry_t));
    ent->h = e;
    if (zip->mode == 'r' || zip->mode == 'a') {
        __kcc_builtin_reset_args();
        __kcc_builtin_add_arg_p(e);
        ent->name = __kcc_builtin_call_p(h, "ziplib_entry_name");
        ent->time = __kcc_builtin_call_i(h, "ziplib_entry_time");
        ent->index = __kcc_builtin_call_i(h, "ziplib_entry_index");
        ent->isdir = __kcc_builtin_call_i(h, "ziplib_entry_isdir");
        ent->isenc = __kcc_builtin_call_i(h, "ziplib_entry_isenc");
        ent->size = __kcc_builtin_call_i(h, "ziplib_entry_size");
        ent->compsize = __kcc_builtin_call_i(h, "ziplib_entry_compsize");
        ent->crc32 = __kcc_builtin_call_i(h, "ziplib_entry_crc32");
        if (!ent->name) {
            ent->name = "(noname)";
        }
    }

    return ent;
}

void zip_entry_close(zip_entry_t *ent)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ent->h);
    __kcc_builtin_call(h, "ziplib_entry_close");
    free(ent);
}

int zip_entry_write(zip_t *zip, const char *name, const void *buf, size_t bufsize)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    __kcc_builtin_add_arg_s(name);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_add_arg_i(bufsize);
    __kcc_builtin_add_arg_i(zip->level);
    int r = __kcc_builtin_call_i(h, "ziplib_entry_write");
    if (r) {
        zip->total_entries++;
    }
    return r;
}

int zip_entry_write_from_file(zip_t *zip, const char *filename)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(zip->h);
    __kcc_builtin_add_arg_s(filename);
    __kcc_builtin_add_arg_s(filename);
    __kcc_builtin_add_arg_i(zip->level);
    int r = __kcc_builtin_call_i(h, "ziplib_entry_write_from_file");
    if (r) {
        zip->total_entries++;
    }
    return r;
}

int zip_entry_read(zip_entry_t *ent, void *buf, size_t bufsize)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ent->h);
    __kcc_builtin_add_arg_p(buf);
    __kcc_builtin_add_arg_i(bufsize);
    return __kcc_builtin_call_i(h, "ziplib_entry_read");
}

int zip_entry_read_to_file(zip_entry_t *ent, const char *filename)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(ent->h);
    __kcc_builtin_add_arg_s(filename);
    return __kcc_builtin_call_i(h, "ziplib_entry_read_to_file");
}

int zip_create(const char *zipname, int level, const char *filenames[], size_t len)
{
    void *h = kcc_extlib();

    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(zipname);
    __kcc_builtin_add_arg_i(level < 1 ? 6 : level);
    for (int i = 0; i < len; ++i) {
        __kcc_builtin_add_arg_s(filenames[i]);
    }
    return __kcc_builtin_call_i(h, "ziplib_create");
}
