
# Vector

## Header

```c
#include <kcc/ext.h>
```

## Type

Use `vector_of_()` macro to declare it.

## Public Members

No public members.

## Public Functions

```c
vector_of_(type, var);
```
*   Declaration of the variable of `var` as vector with the type of `type`.

```c
void vector_free(obj);
```
*   Frees a vector object.
*   Do not access to the freed object.

```c
int vector_size(obj);
```
*   Returns the element count of `obj`.

```c
void vector_resize(obj, size);
```
*   Resizes `obj` to append `size` elements more.

```c
void vector_push(obj, value);
```
*   Appends `value` to the tail of `obj`.

```c
void vector_unshift(obj, value);
```
*   Appends `value` to the head of `obj`.

```c
void vector_pop(obj);
```
*   Removes the last element of `obj`.

```c
void vector_head(obj);
```
*   Accesses the head element of `obj`.

```c
void vector_last(obj);
```
*   Accesses the last element of `obj`.

```c
void vector_shrinkto(obj, size);
```
*   Shrinks `obj` down to `size`.

```c
void vector_shrinkby(obj, size);
```
*   Shrinks `obj` down by `size`.

```c
void vector_remove(obj, index, count);
```
*   Removes elements from `obj` with `count` from `index`.
