#ifndef ZIP_H
#define ZIP_H

#include <_ext.h>
#include <string.h>
#include <stdint.h>

#define ZIP_DEFAULT_COMPRESSION_LEVEL 6
typedef struct zip_ {
    void *h;
    int level;
    char mode;
    int total_entries;
} zip_t;

typedef struct zip_entry_ {
    void *h;
    int index;
    int isdir;
    int isenc;
    time_t time;
    const char *name;
    unsigned long long size;
    unsigned long long compsize;
    unsigned int crc32;
} zip_entry_t;

zip_t *zip_open(const char *zipname, char mode);
zip_t *zip_compression_level(zip_t *zip, int level);
void zip_close(zip_t *zip);
zip_entry_t *zip_entry_open(zip_t *zip, const char *entryname);
zip_entry_t *zip_entry_openbyindex(zip_t *zip, int index);
void zip_entry_close(zip_entry_t *ent);
int zip_entry_write(zip_t *zip, const char *name, const void *buf, size_t bufsize);
int zip_entry_write_from_file(zip_t *zip, const char *filename);
int zip_entry_read(zip_entry_t *ent, void *buf, size_t bufsize);
int zip_entry_read_to_file(zip_entry_t *ent, const char *filename);
int zip_create(const char *zipname, int level, const char *filenames[], size_t len);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcs/ext_zip.c>
#endif
#endif

#endif /* ZIP_H */
