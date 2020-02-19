
# SQLite3

## Header

```c
#include <kcs/sqlite3.h>
```

## Type

`sqlite3_t` for database, and `sqlite3_stmt_t` for statement.

## Public Members

No public members.

## Public Functions

### Open/Close

```c
sqlite3_t *sqlite3_open(const char *filename, int timeout);
```
*   Opens the database with `filename`.
*   `timeout` for busy timeout in millisec.
*   `SQLITE_NO_TIMEOUT` for no timeout.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_close(sqlite3_t *db);
```
*   Closes the database.
*   Do not use a closed database.
*   Returns `SQLITE_OK` for successful.

### Statement

```c
sqlite3_stmt_t *sqlite3_prepare(sqlite3_t *db, const char *sql);
```
*   Prepares the statement object created by `sql`.

```c
void sqlite3_finalize(sqlite3_stmt_t *stmt);
```
*   Closes the statement.
*   Do not use a closed statement.

```c
int sqlite3_clear_bindings(sqlite3_stmt_t *stmt);
```
*   Clears the all bound values.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_reset(sqlite3_stmt_t *stmt);
```
*   Resets the statement state.
*   Bound data would not be cleared, use `sqlite3_clear_bindings` for that purpose.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_step(sqlite3_stmt_t *stmt);
```
*   Executes a step for the statement.
*   `SQLITE_ROW` means the success of reading a row.
*   `SQLITE_DONE` means reading has been done until the end.

### Binder

```c
int sqlite3_bind_null(sqlite3_stmt_t *stmt, int index);
```
*   Binds `NULL` to the placeholder specified by the `index`.
*   `index` should be started from 1.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_bind_blob(sqlite3_stmt_t *stmt, int index, void *blob, int len, int mkcopy);
```
*   Binds blob data to the placeholder specified by the `index`.
*   `index` should be started from 1.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_bind_double(sqlite3_stmt_t *stmt, int index, double val);
```
*   Binds a double value to the placeholder specified by the `index`.
*   `index` should be started from 1.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_bind_int(sqlite3_stmt_t *stmt, int index, int val);
```
*   Binds a integer value to the placeholder specified by the `index`.
*   `index` should be started from 1.
*   Returns `SQLITE_OK` for successful.

```c
int sqlite3_bind_text(sqlite3_stmt_t *stmt, int index, const char *val, int len);
```
*   Binds a text value to the placeholder specified by the `index`.
*   `index` should be started from 1.
*   Returns `SQLITE_OK` for successful.

### Accessor

```c
int sqlite3_column_count(sqlite3_stmt_t *stmt);
```
*   Returns the column count of row.

```c
int sqlite3_column_type(sqlite3_stmt_t *stmt, int index);
```
*   Returns the type of the column specified by the `index`.
*   `index` should be started from 0.
*   Types should be:
    *   `SQLITE_INTEGER` for integer.
    *   `SQLITE_FLOAT` for double.
    *   `SQLITE_TEXT` for text string.
    *   `SQLITE_BLOB` for blob data.
    *   `SQLITE_NULL` for `NULL`.

```c
int sqlite3_column_bytes(sqlite3_stmt_t *stmt, int index);
```
*   Returns the data bytes of the column specified by the `index`.
*   `index` should be started from 0.

```c
int64_t sqlite3_column_int(sqlite3_stmt_t *stmt, int index);
```
*   Returns a integer value of the column specified by the `index`.
*   `index` should be started from 0.

```c
double sqlite3_column_double(sqlite3_stmt_t *stmt, int index);
```
*   Returns a double value of the column specified by the `index`.
*   `index` should be started from 0.

```c
void *sqlite3_column_blob(sqlite3_stmt_t *stmt, int index);
```
*   Returns blob data of the column specified by the `index`.
*   `index` should be started from 0.

```c
const char *sqlite3_column_text(sqlite3_stmt_t *stmt, int index);
```
*   Returns a text value of the column specified by the `index`.
*   `index` should be started from 0.

```c
const char *sqlite3_column_name(sqlite3_stmt_t *stmt, int index);
```
*   Returns the name of the column specified by the `index`.
*   `index` should be started from 0.

### Utility

```c
int sqlite3_exec(sqlite3_t *db, const char *sql);
```
*   Executes the sql of `sql` in one action.
*   Returns `SQLITE_OK` for successful.

```c
const char *sqlite3_last_errmsg(sqlite3_t *db);
```
*   Returns a last error message.
