
# Big Integer

## Header

```c
#include <kcc/bigint_t.h>
```

## Type

`bigint_t`.

## Public Members

| Name  | Type  |        Meaning        |
| :---- | :---- | :-------------------- |
| `neg` | `int` | false(0) if positive. |

## Public Functions

### Initialization/Finalization

```c
void bigint_init(obj);
```
*   Initializes a big integer object of `obj`.

```c
void bigint_free(obj);
```
*   Frees a big integer object of `obj`.
*   Do not access to the freed object.

### Creation

```c
bigint_t* bigint_from_str_base(bigint_t *dst, const char *src, int src_base);
```
*   Creates `bigint_t` value from a string source with the base.

```c
bigint_t* bigint_from_str(bigint_t *dst, const char *src);
```
*   Creates `bigint_t` value from a string source.

```c
bigint_t* bigint_from_int(bigint_t *dst, int src);
```
*   Creates `bigint_t` value from an integer source.

```c
bigint_t* bigint_from_word(bigint_t *dst, bigint_word a);
```
*   Creates `bigint_t` value from a 32bit integer source.

```c
bigint_t* bigint_set_neg(bigint_t *dst, int neg);
```
*   Sets `neg` for `dst`.

```c
bigint_t* bigint_negate(bigint_t *dst);
```
*   Negates `dst`.
*   Makes it positive if `dst` was negative.
*   Makes it negative if `dst` was positive.

### Comparison

```c
int bigint_cmp(const bigint_t *a, const bigint_t *b);
```
*   0 for `a == b`.
*   +1 for `a > b`.
*   -1 for `a < b`.

### Calculation

```c
bigint_t* bigint_add(bigint_t *dst, const bigint_t *a, const bigint_t *b);
```
*   `dst = a + b`
*   Both `a` and `b` are the `bigint_t` object.

```c
bigint_t* bigint_add_word(bigint_t *dst, const bigint_t *src_a, bigint_word b);
```
*   `dst = a + b`
*   `b` is a 32bit integer.

```c
bigint_t* bigint_sub(bigint_t *dst, const bigint_t *a, const bigint_t *b);
```
*   `dst = a - b`
*   Both `a` and `b` are the `bigint_t` object.

```c
bigint_t* bigint_sub_word(bigint_t *dst, const bigint_t *src_a, bigint_word b);
```
*   `dst = a - b`
*   `b` is a 32bit integer.

```c
bigint_t* bigint_mul(bigint_t *dst, const bigint_t *a, const bigint_t *b);
```
*   `dst = a * b`
*   Both `a` and `b` are the `bigint_t` object.

```c
bigint_t* bigint_div_mod(
    bigint_t *dst_quotient,
    bigint_t *dst_remainder,
    const bigint_t *src_numerator,
    const bigint_t *src_denominator
);
```
*   `dst_quotient = src_numerator / src_denominator`
*   `dst_remainder = src_numerator % src_denominator`

```c
bigint_t* bigint_div(
    bigint_t *dst,
    const bigint_t *numerator,
    const bigint_t *denominator
);
```
*   `dst = numerator / denominator`

```c
bigint_t* bigint_mod(
    bigint_t *dst,
    const bigint_t *numerator,
    const bigint_t *denominator
);
```
*   `dst = numerator % denominator`

```c
bigint* bigint_pow_word(bigint *dst, const bigint *src, bigint_word exponent);
```
*   `dst = src ^ exponent`
*   `exponent` is a 32bit integer.

```c
bigint_t* bigint_sqrt(bigint_t *dst, const bigint_t *src);
```
*   `dst = sqrt(src)`

### Utility

```c
bigint_t* bigint_cpy(bigint_t *dst, const bigint_t *src);
```
*   `dst = copy(src)`
*   `dst` should be freed by `bigint_free` after used.

```c
double bigint_double(const bigint *src);
```
*   Returns the value as `double`.

```c
char* bigint_write(char *dst, int n_dst, const bigint *a);
```
*   Returns the value as `char *`.
*   `dst` buffer should be enough for displaying it.
