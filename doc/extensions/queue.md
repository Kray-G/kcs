
# Queue

## Header

```c
#include <kcc/ext.h>
```

## Type

Use `queue_of_()` macro to declare it.

## Public Members

No public members.

## Public Functions

```c
queue_of_(type, var);
```
*   Declaration of the variable of `var` as queue with the type of `type`.

```c
void queue_free(obj);
```
*   Frees a queue object.
*   Do not access to the freed object.

```c
int queue_size(obj);
```
*   Returns the element count of `obj`.

```c
void queue_enqueue(obj, value);
```
*   Appends `value` to the head of `obj`.

```c
type queue_dequeue(obj);
```
*   Removes the last element from `obj`, and returns it.
