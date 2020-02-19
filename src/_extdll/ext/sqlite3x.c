#include <kcs/dll.h>
#include <kcs/dllcore.h>
#include "../lib/sqlite3/sqlite3.h"
#include <string.h>

static char *g_last_error = NULL;

static void sqliye3lib_clear_errmsg(void)
{
    if (g_last_error) {
        sqlite3_free(g_last_error);
        g_last_error = NULL;
    }
}

DLLEXPORT void *sqlite3lib_open(int argc, arg_type_t *argv)
{
    if (argc != 1 || argv[0].type != C_STR) {
        return NULL;
    }

    const unsigned char *filename = (const unsigned char *)argv[0].value.s;
    sqlite3 *db = NULL;
    int r = sqlite3_open(filename, &db);
    if (r != SQLITE_OK) {
        return NULL;
    }
    return db;
}

DLLEXPORT int sqlite3lib_busy_timeout(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3 *db = (sqlite3 *)argv[0].value.p;
    int tmout = (int)argv[1].value.i;
    return sqlite3_busy_timeout(db, tmout);
}

DLLEXPORT int sqlite3lib_close(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return SQLITE_ERROR;
    }

    sqlite3 *db = (sqlite3 *)argv[0].value.p;
    return sqlite3_close(db);
}

DLLEXPORT const char *sqlite3lib_last_errmsg(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return "Invalid parameters";
    }
    sqlite3 *db = (sqlite3 *)argv[0].value.p;
    return sqlite3_errmsg(db);
}

DLLEXPORT int sqlite3lib_exec(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_STR) {
        return SQLITE_ERROR;
    }

    sqlite3 *db = (sqlite3 *)argv[0].value.p;
    const char *sql = (const char *)argv[1].value.s;
    return sqlite3_exec(db, sql, NULL, NULL, &g_last_error);
}

DLLEXPORT void *sqlite3lib_prepare(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_STR) {
        return NULL;
    }

    sqlite3 *db = (sqlite3 *)argv[0].value.p;
    const char *sql = (const char *)argv[1].value.s;
    sqlite3_stmt *stmt = NULL;
    int r = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (r != SQLITE_OK) {
        return NULL;
    }
    return stmt;
}

DLLEXPORT void sqlite3lib_finalize(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    sqlite3_finalize(stmt);
}

DLLEXPORT int sqlite3lib_bind_null(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_bind_null(stmt, index);
}

DLLEXPORT int sqlite3lib_bind_blob(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 5 || argv[0].type != C_PTR || argv[1].type != C_INT || argv[2].type != C_PTR || argv[3].type != C_INT || argv[4].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    void *blob = (void *)argv[2].value.p;
    int len = (int)argv[3].value.i;
    int copy = (int)argv[4].value.i;
    return sqlite3_bind_blob(stmt, index, blob, len, copy ? SQLITE_TRANSIENT : SQLITE_STATIC);
}

DLLEXPORT int sqlite3lib_bind_double(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 3 || argv[0].type != C_PTR || argv[1].type != C_INT || argv[2].type != C_DBL) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    double val = argv[2].value.d;
    return sqlite3_bind_double(stmt, index, val);
}

DLLEXPORT int sqlite3lib_bind_int(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 3 || argv[0].type != C_PTR || argv[1].type != C_INT || argv[2].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    int64_t val = argv[2].value.i;
    return sqlite3_bind_int64(stmt, index, val);
}

DLLEXPORT int sqlite3lib_bind_text(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 4 || argv[0].type != C_PTR || argv[1].type != C_INT || argv[2].type != C_STR || argv[3].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    const char *val = argv[2].value.s;
    int len = (int)argv[3].value.i;
    return sqlite3_bind_text(stmt, index, val, len < 0 ? strlen(val) : len, NULL);
}

DLLEXPORT int sqlite3lib_step(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    return sqlite3_step(stmt);
}

DLLEXPORT int sqlite3lib_clear_bindings(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    return sqlite3_clear_bindings(stmt);
}

DLLEXPORT int sqlite3lib_reset(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    return sqlite3_reset(stmt);
}

DLLEXPORT int sqlite3lib_column_count(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 1 || argv[0].type != C_PTR) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    return sqlite3_column_count(stmt);
}

DLLEXPORT int sqlite3lib_column_type(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_type(stmt, index);
}

DLLEXPORT int sqlite3lib_column_bytes(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_bytes(stmt, index);
}

DLLEXPORT int64_t sqlite3lib_column_int(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_int64(stmt, index);
}

DLLEXPORT double sqlite3lib_column_double(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return SQLITE_ERROR;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_double(stmt, index);
}

DLLEXPORT const void *sqlite3lib_column_blob(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return NULL;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_blob(stmt, index);
}

DLLEXPORT const char *sqlite3lib_column_text(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return NULL;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_text(stmt, index);
}

DLLEXPORT const char *sqlite3lib_column_name(int argc, arg_type_t *argv)
{
    sqliye3lib_clear_errmsg();
    if (argc != 2 || argv[0].type != C_PTR || argv[1].type != C_INT) {
        return NULL;
    }

    sqlite3_stmt *stmt = (sqlite3_stmt *)argv[0].value.p;
    int index = (int)argv[1].value.i;
    return sqlite3_column_name(stmt, index);
}
