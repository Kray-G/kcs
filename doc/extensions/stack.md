
# Stack

## Header

```c
#include <kcc/ext.h>
```

## Type

Use `stack_of_()` macro to declare it.

## Public Members

No public members.

## Public Functions

```c
stack_of_(type, var);
```
*   Declaration of the variable of `var` as stack with the type of `type`.

```c
void stack_free(obj);
```
*   Frees a stack object.
*   Do not access to the freed object.

```c
int stack_size(obj);
```
*   Returns the element count of `obj`.

```c
void stack_push(obj, value);
```
*   Appends `value` to the tail of `obj`.

```c
type stack_pop(obj);
```
*   Removes the last element from `obj`, and returns it.
