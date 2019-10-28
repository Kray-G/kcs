
# Regular Expression

## Header

```c
#include <kcc/regex.h>
```

## Type

`regex_t`.

## Public Members

|     Name     | Type  |                     Meaning                      |
| :----------- | :---- | :----------------------------------------------- |
| `num_regs`   | `int` | The number of captured string.                   |
| `beg[index]` | `int` | The start point of captured string with `index`. |
| `end[index]` | `int` | The end point of captured string with `index`.   |

## Public Functions

```c
regex_t *regex_compile(const char *pattern);
```
*   Compiles `pattern` as a regular expression string.

```c
int regex_search(regex_t *regex, const char *str);
```
*   Searches the text matched to the pattern from `str`.
*   `true(1)` if found, otherwise `false(0)`.
*   Searching from the head of `str` at the first time.
*   After the next time, continuing it from the next position.

```c
void regex_free(regex_t *regex);
```
*   Frees the object.
*   Do not access to the freed object.
