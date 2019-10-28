#include <kcc/sqlite3.h>
#include <stdlib.h>

sqlite3_t *sqlite3_open(const char *filename, int timeout)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_s(filename);
    sqlite3_t *db = __kcc_builtin_call_p(h, "sqlite3lib_open");
    if (timeout >= 0) {
        __kcc_builtin_reset_args();
        __kcc_builtin_add_arg_i(timeout);
        int r = __kcc_builtin_call_i(h, "sqlite3lib_busy_timeout");
        if (r != SQLITE_OK) {
            sqlite3_close(db);
            return NULL;
        }
    }
    return (sqlite3_t *)db;
}

int sqlite3_close(sqlite3_t *db)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(db);
    return __kcc_builtin_call_i(h, "sqlite3lib_close");
}

const char *sqlite3_last_errmsg(sqlite3_t *db)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(db);
    return __kcc_builtin_call_p(h, "sqlite3lib_last_errmsg");
}

int sqlite3_exec(sqlite3_t *db, const char *sql)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(db);
    __kcc_builtin_add_arg_s(sql);
    return __kcc_builtin_call_i(h, "sqlite3lib_exec");
}

sqlite3_stmt_t *sqlite3_prepare(sqlite3_t *db, const char *sql)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(db);
    __kcc_builtin_add_arg_s(sql);
    return (sqlite3_stmt_t *)__kcc_builtin_call_p(h, "sqlite3lib_prepare");
}

void sqlite3_finalize(sqlite3_stmt_t *stmt)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_call(h, "sqlite3lib_finalize");
}

int sqlite3_bind_null(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_i(h, "sqlite3lib_bind_null");
}

int sqlite3_bind_blob(sqlite3_stmt_t *stmt, int index, void *blob, int len, int mkcopy)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    __kcc_builtin_add_arg_p(blob);
    __kcc_builtin_add_arg_i(len);
    __kcc_builtin_add_arg_i(mkcopy);
    return __kcc_builtin_call_i(h, "sqlite3lib_bind_blob");
}

int sqlite3_bind_double(sqlite3_stmt_t *stmt, int index, double val)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    __kcc_builtin_add_arg_d(val);
    return __kcc_builtin_call_i(h, "sqlite3lib_bind_double");
}

int sqlite3_bind_int(sqlite3_stmt_t *stmt, int index, int val)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    __kcc_builtin_add_arg_i(val);
    return __kcc_builtin_call_i(h, "sqlite3lib_bind_int");
}

int sqlite3_bind_text(sqlite3_stmt_t *stmt, int index, const char *val, int len)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    __kcc_builtin_add_arg_s(val);
    __kcc_builtin_add_arg_i(len);
    return __kcc_builtin_call_i(h, "sqlite3lib_bind_text");
}

int sqlite3_step(sqlite3_stmt_t *stmt)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    return __kcc_builtin_call_i(h, "sqlite3lib_step");
}

int sqlite3_clear_bindings(sqlite3_stmt_t *stmt)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    return __kcc_builtin_call_i(h, "sqlite3lib_clear_bindings");
}

int sqlite3_reset(sqlite3_stmt_t *stmt)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    return __kcc_builtin_call_i(h, "sqlite3lib_reset");
}

int sqlite3_column_count(sqlite3_stmt_t *stmt)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    return __kcc_builtin_call_i(h, "sqlite3lib_column_count");
}

int sqlite3_column_type(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_i(h, "sqlite3lib_column_type");
}

int sqlite3_column_bytes(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_i(h, "sqlite3lib_column_bytes");
}

int64_t sqlite3_column_int(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_i(h, "sqlite3lib_column_int");
}

double sqlite3_column_double(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_d(h, "sqlite3lib_column_double");
}

void *sqlite3_column_blob(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_p(h, "sqlite3lib_column_blob");
}

const char *sqlite3_column_text(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_p(h, "sqlite3lib_column_text");
}

const char *sqlite3_column_name(sqlite3_stmt_t *stmt, int index)
{
    void *h = kcc_extlib();
    __kcc_builtin_reset_args();
    __kcc_builtin_add_arg_p(stmt);
    __kcc_builtin_add_arg_i(index);
    return __kcc_builtin_call_p(h, "sqlite3lib_column_name");
}
