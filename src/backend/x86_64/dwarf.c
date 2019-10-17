#include <kcc.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "dwarf.h"
#include "elf.h"
#include <lacc/array.h>
#include <lacc/context.h>

#include <kcc/assert.h>
#include <stdarg.h>

/* Unit header. Only generate compile units. */
#define DW_UT_compile 0x01

/* Tag encodings. */
#define DW_TAG_compile_unit 0x11

/* Child property of entry. */
#define DW_CHILDREN_no 0x00
#define DW_CHILDREN_yes 0x01

/* Attribute encoding. */
#define DW_AT_name 0x03
#define DW_AT_language 0x13
#define DW_AT_producer 0x25
#define DW_AT_comp_dir 0x1b
#define DW_AT_stmt_list 0x10 /* lineptr */
#define DW_AT_low_pc 0x11
#define DW_AT_high_pc 0x12

/* Attribute form encoding. */
#define DW_FORM_string 0x08
#define DW_FORM_data1 0x0b
#define DW_FORM_data2 0x05

/* Language names. */
#define DW_LANG_C89 0x0001
#define DW_LANG_C99 0x000c
#define DW_LANG_C11 0x001d

/* Attribute of Debug information entry. */
struct dwarf_attr {
    int name; /* DW_AT_* */
    int form; /* DW_FORM_* */
    union {
        const char *str;
        int64_t num;
        uint64_t unum;
    } value;
};

/* Debugging information entry (DIE). */
struct dwarf_die {
    int code;
    int tag; /* DW_TAG_* */
    array_of(struct dwarf_attr) attributes;
    array_of(struct dwarf_die *) children;
};

/* Each compile unit has a root DIE with code DW_UT_compile. */
static struct dwarf_die *dwarf_root_die;

static size_t ULEB128_encode(uint8_t *buf, uint64_t value)
{
    size_t i;
    uint8_t b;

    i = 0;
    do {
        i++;
        b = value & 0x7f;
        value >>= 7;
        if (value) {
            b |= 0x80;
        }
        *buf = b;
        buf += 1;
    } while (value);

    return i;
}

static void dwarf_write_ULEB128(int shid, uint64_t value)
{
    uint8_t buf[8];
    size_t len;

    len = ULEB128_encode(buf, value);
    elf_section_write(shid, buf, len);

    assert(len == 1);
}

static struct dwarf_die *dwarf_add_entry(int code, int tag)
{
    struct dwarf_die *die;

    die = calloc(1, sizeof(*die));
    die->code = code;
    die->tag = tag;
    return die;
}

static void dwarf_add_attribute(struct dwarf_die *die, int name, int form, ...)
{
    struct dwarf_attr attr = {0};
    va_list args;

    attr.name = name;
    attr.form = form;

    va_start(args, form);
    switch (form) {
    case DW_FORM_data1:
        attr.value.num = va_arg(args, int);
        break;
    case DW_FORM_string:
        attr.value.str = va_arg(args, const char *);
        break;
    default: assert(0);
        break;
    }

    va_end(args);
    array_push_back(&die->attributes, attr);
}

static void dwarf_write_attribute(
    const struct dwarf_attr *attr,
    int shdi,
    int shda)
{
    size_t size;

    if (!attr) {
        dwarf_write_ULEB128(shda, 0);
        dwarf_write_ULEB128(shda, 0);
    } else {
        dwarf_write_ULEB128(shda, attr->name);
        dwarf_write_ULEB128(shda, attr->form);

        switch (attr->form) {
        default: assert(0);
        case DW_FORM_string:
            size = strlen(attr->value.str) + 1;
            elf_section_write(shdi, attr->value.str, size);
            break;
        case DW_FORM_data1:
            elf_section_write(shdi, &attr->value.num, 1);
            break;
        }
    }
}

static void dwarf_write_entry(struct dwarf_die *die, int shdi, int shda)
{
    int i;
    struct dwarf_attr *attr;
    struct dwarf_die *child;
    uint8_t children[] = {DW_CHILDREN_no, DW_CHILDREN_yes};

    assert(die);
    dwarf_write_ULEB128(shdi, die->code);
    dwarf_write_ULEB128(shda, die->code);
    dwarf_write_ULEB128(shda, die->tag);
    elf_section_write(shda, &children[array_len(&die->children) != 0], 1);

    for (i = 0; i < array_len(&die->attributes); ++i) {
        attr = &array_get(&die->attributes, i);
        dwarf_write_attribute(attr, shdi, shda);
    }

    dwarf_write_attribute(NULL, shdi, shda);

    for (i = 0; i < array_len(&die->children); ++i) {
        child = array_get(&die->children, i);
        dwarf_write_entry(child, shdi, shda);
    }

    array_clear(&die->attributes);
    array_clear(&die->children);
    free(die);
}

INTERNAL int dwarf_init(const char *filename)
{
    struct dwarf_die *die;

    die = dwarf_add_entry(DW_UT_compile, DW_TAG_compile_unit);
    dwarf_add_attribute(die, DW_AT_name, DW_FORM_string, filename);
    dwarf_add_attribute(die, DW_AT_producer, DW_FORM_string, "lacc");
    switch (context.standard) {
    case STD_C89:
        dwarf_add_attribute(die, DW_AT_language, DW_FORM_data1, DW_LANG_C89);
        break;
    case STD_C99:
        dwarf_add_attribute(die, DW_AT_language, DW_FORM_data1, DW_LANG_C99);
        break;
    case STD_C11:
        dwarf_add_attribute(die, DW_AT_language, DW_FORM_data1, DW_LANG_C11);
        break;
    }

    dwarf_root_die = die;
    return 0;
}

INTERNAL int dwarf_flush(void)
{
    int di, da;
    char *buffer;
    size_t length;

    uint32_t padding = 0xffffffffu;
    uint64_t unit_length = 0;
    uint16_t version = 4;
    uint64_t debug_abbrev_offset = 0;
    uint16_t address_size = 8;

    di = elf_section_init(".debug_info", SHT_PROGBITS, 0, SHN_UNDEF, 0, 8, 0);
    da = elf_section_init(".debug_abbrev", SHT_PROGBITS, 0, SHN_UNDEF, 0, 8, 0);

    elf_section_write(di, &padding, 4);
    elf_section_write(di, &unit_length, 8);
    elf_section_write(di, &version, 2);
    elf_section_write(di, &debug_abbrev_offset, 8);
    elf_section_write(di, &address_size, 1);

    /* Write all entries with children recursively. */
    dwarf_write_entry(dwarf_root_die, di, da);

    /* End with zero byte. */
    length = elf_section_write(di, NULL, 1);

    /*
     * Length of .debug_data contribution for this compilation unit,
     * not including itself or padding
     */
    length = length - 12;
    buffer = elf_section_buffer(di);
    memcpy(buffer + 4, &length, 8);
    return 0;
}
