
# String

## Header

```c
#include <kcc/ext.h>
```

## Type

`string_t`.

## Public Members

|  Name  |   Type   |              Meaning               |
| :----- | :------- | :--------------------------------- |
| `len`  | `int`    | The length of string.              |
| `cstr` | `char *` | The pointer to the head of string. |

## Public Functions

```c
string_t string_init(const char *cstr);
```
*   Initializes `string_t` object from `cstr`.

```c
string_t string_copy(const string_t rhs);
```
*   Copies `string_t` object and returns a new `string_t` object from `rhs`.
*   Returned value should be freed by `string_free()`.

```c
void string_append(string_t* lhs, const string_t rhs);
```
*   Appends `rhs` object to `*lhs` object.
*   `*lhs` object is overwritten by a new string data information.

```c
void string_append_cstr(string_t* lhs, const char *rhs);
```
*   Appends `rhs` in the c-style string to `*lhs` object.
*   `*lhs` object is overwritten by a new string data.

```c
string_t string_substr(const string_t str, int start, int len);
```
*   Returns a new `string_t` object from `start` with the length of `len`.
*   Returned value should be freed by `string_free()`.

```c
void string_free(string_t *str);
```
*   `string_t` object will be freed.
*   Do not use it after freed.

```c
void string_clear(string_t *str);
```
*   Make the `string_t` object empty.
