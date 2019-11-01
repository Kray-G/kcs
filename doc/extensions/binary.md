
# Binary

## Header

```c
#include <kcc/ext.h>
```

## Type

`binary_t`.

## Public Members

| Name  |       Type        |              Meaning               |
| :---- | :---------------- | :--------------------------------- |
| `len` | `int`             | The length of binary.              |
| `buf` | `unsigned char *` | The pointer to the head of binary. |

## Public Functions

```c
binary_t binary_init(const unsigned char *buf, int len);
```
*   Initializes `binary_t` object from `buf` with the length of `len`.

```c
binary_t binary_copy(const binary_t rhs);
```
*   Copies `binary_t` object and returns a new `binary_t` object from `rhs`.
*   Returned value should be freed by `binary_free()`.

```c
void binary_append(binary_t* lhs, const binary_t rhs);
```
*   Appends `rhs` object to `*lhs` object.
*   `*lhs` object is overwritten by a new binary data information.

```c
void binary_append_bytes(binary_t* lhs, const unsigned char *rhs, int len);
```
*   Appends `rhs` of the byte array to `*lhs` object.
*   `*lhs` object is overwritten by a new binary data.

```c
binary_t binary_subbin(const binary_t bin, int start, int len);
```
*   Returns a new `binary_t` object from `start` with the length of `len`.
*   Returned value should be freed by `binary_free()`.

```c
void binary_free(binary_t *bin);
```
*   `binary_t` object will be freed.
*   Do not use it after freed.

```c
void binary_clear(binary_t *bin);
```
*   Make the `binary_t` object empty.
