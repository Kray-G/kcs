
# Zip/Unzip

## Header

```c
#include <kcs/zip.h>
```

## Type

`zip_t` for zip file, and `zip_entry_t` for each entry.

## Public Members

### `zip_t`

|     Name      |  Type  |               Meaning                |
| :------------ | :----- | ------------------------------------ |
| level         | `int`  | Current compression level to be set. |
| mode          | `char` | Current open mode to be set.         |
| total_entries | `int`  | The number of entries in a zip file. |

### `zip_entry_t`

|   Name   |         Type         |                 Meaning                  |
| :------- | :------------------- | :--------------------------------------- |
| index    | `int`                | The index of the entry.                  |
| isdir    | `int`                | `false(0)` for not directory.            |
| isenc    | `int`                | `false(0)` for no encryption data.       |
| time     | `time_t`             | The time of the entry data.              |
| name     | `const char *`       | The name of the entry data.              |
| size     | `unsigned long long` | The uncompressed size of the entry data. |
| compsize | `unsigned long long` | The compressed size of the entry data.   |
| crc32    | `unsigned int`       | The CRC data of the entry data.          |

## Public Functions

### Open/Close for Zip File

```c
zip_t *zip_open(const char *zipname, char mode);
```
*   Opens the zip file with the mode.
*   The modes are:
    *   `'r'` for Read Mode from the zip file.
    *   `'w'` for Write Mode to the zip file.
    *   `'a'` for Append Mode to the zip file.

```c
void zip_close(zip_t *zip);
```
*   Closes the zip file.
*   Do not use a closed zip file.

### Setup

```c
zip_t *zip_compression_level(zip_t *zip, int level);
```
*   Sets a compression level.
*   It would be 6 by default.

### Open/Close for Entry

```c
zip_entry_t *zip_entry_open(zip_t *zip, const char *entryname);
```
*   Opens the entry by the entry's name.

```c
zip_entry_t *zip_entry_openbyindex(zip_t *zip, int index);
```
*   Opens the entry by the entry's index.

```c
void zip_entry_close(zip_entry_t *ent);
```
*   Closes the zip entry.
*   Do not access to the closed zip entry.

### Accessor

```c
int zip_entry_write(zip_t *zip, const char *name, const void *buf, size_t bufsize);
```
*   Appends an entry to `zip` file from memory buffer.
*   Returns `false(0)` for failed.

```c
int zip_entry_write_from_file(zip_t *zip, const char *filename);
```
*   Appends an entry to `zip` file from the file of `filename`.
*   Returns `false(0)` for failed.

```c
int zip_entry_read(zip_entry_t *ent, void *buf, size_t bufsize);
```
*   Reads a data of the entry to the memory buffer.
*   Returns `false(0)` for failed.

```c
int zip_entry_read_to_file(zip_entry_t *ent, const char *filename);
```
*   Reads a data of the entry and write it to the file of `filename`.
*   Returns `false(0)` for failed.

### Utility

```c
int zip_create(const char *zipname, int level, const char *filenames[], size_t len);
```
*   Creates a zip file with specified files.
