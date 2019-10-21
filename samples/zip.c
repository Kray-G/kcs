#include <stdio.h>
#include <time.h>
#include <kcc/zip.h>
const char *filenames[] = {
    "README.md",
    "LICENSE",
    "Makefile",
    "Makefile.msc",
    "make.cmd",
};

void zip_create_test()
{
    zip_create("test_zip_created.zip",
               6, // Compression level.
               filenames,
               sizeof(filenames)/sizeof(filenames[0]));
}

void zip_new_test(const char *name)
{
    zip_t *z = zip_open(name, 'w');
    if (z) {
        int len = sizeof(filenames)/sizeof(filenames[0]);
        for (int i = 0; i < len; ++i) {
            if (!zip_entry_write_from_file(z, filenames[i])) {
                printf("failed: %s\n", filenames[i]);
            }
        }
        const char *text = "This is a pen!\nNo problem.\n";
        if (!zip_entry_write(z, "text.txt", text, strlen(text))) {
            printf("failed: text.txt\n");
        }
        zip_close(z);
    }
}

void zip_append_test(const char *name)
{
    const char *appendfile[] = {
        "doc/cstdlib.md",
        "include/kcc.h",
    };
    zip_t *z = zip_open(name, 'a');
    if (z) {
        int len = sizeof(appendfile)/sizeof(appendfile[0]);
        for (int i = 0; i < len; ++i) {
            if (!zip_entry_write_from_file(z, appendfile[i])) {
                printf("failed: %s\n", appendfile[i]);
            }
        }
        for (int i = 0; i < z->total_entries; ++i) {
            zip_entry_t *e = zip_entry_openbyindex(z, i);
            if (e)  {
                printf("--------\n");
                printf("index = %d\n", e->index);
                printf("time  = %s", asctime(localtime(&e->time)));
                printf("name  = %s\n", e->name);
                printf("size  = %llu\n", e->size);
                printf("comps = %llu\n", e->compsize);
                printf("isdir = %d\n", e->isdir);
                printf("isenc = %d\n", e->isenc);
                printf("crc32 = %X\n", e->crc32);
                zip_entry_close(e);
            }
        }
        zip_close(z);
    }
}

void zip_list(const char *name)
{
    zip_t *z = zip_open(name, 'r');
    if (z) {
        printf("mode = %c\n", z->mode);
        printf("entries = %d\n", z->total_entries);
        for (int i = 0; i < z->total_entries; ++i) {
            zip_entry_t *e = zip_entry_openbyindex(z, i);
            if (e)  {
                printf("--------\n");
                printf("index = %d\n", e->index);
                printf("time  = %s", asctime(localtime(&e->time)));
                printf("name  = %s\n", e->name);
                printf("size  = %llu\n", e->size);
                printf("comps = %llu\n", e->compsize);
                printf("isdir = %d\n", e->isdir);
                printf("isenc = %d\n", e->isenc);
                printf("crc32 = %X\n", e->crc32);
                if (e->size < 512) {
                    char buf[512] = {0};
                    if (zip_entry_read(e, buf, 511)) {
                        printf("--------\n");
                        printf("%s\n", buf);
                    }
                    // char filename[512] = {0};
                    // sprintf(filename, "output%d.txt", i);
                    // if (!zip_entry_read_to_file(e, filename)) {
                    //     printf("output failed: %s\n", filename);
                    // }
                }
                zip_entry_close(e);
            }
        }
        zip_close(z);
    }
}

void zip_info(const char *name, const char *entryname)
{
    zip_t *z = zip_open(name, 'r');
    if (z) {
        zip_entry_t *e = zip_entry_open(z, entryname);
        if (e)  {
            printf("--------\n");
            printf("index = %d\n", e->index);
            printf("time  = %s", asctime(localtime(&e->time)));
            printf("name  = %s\n", e->name);
            printf("size  = %llu\n", e->size);
            printf("comps = %llu\n", e->compsize);
            printf("isdir = %d\n", e->isdir);
            printf("isenc = %d\n", e->isenc);
            printf("crc32 = %X\n", e->crc32);
            zip_entry_close(e);
        }
        zip_close(z);
    }
}

int main()
{
    printf("-------- Creating zip: test_zip_created.zip.\n");
    zip_create_test();
    printf("-------- Listing zip: test_zip_created.zip.\n");
    zip_list("test_zip_created.zip");
    printf("-------- Specified by a file name.\n");
    zip_info("test_zip_created.zip", "make.cmd");

    printf("-------- Creating zip: test_zip_new.zip.\n");
    zip_new_test("test_zip_new.zip");
    zip_append_test("test_zip_new.zip");
    printf("-------- Listing zip: test_zip_new.zip.\n");
    zip_list("test_zip_new.zip");
    return 0;
}

