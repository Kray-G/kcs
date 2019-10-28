#ifndef KCC_SQLITE3_H
#define KCC_SQLITE3_H

#include <_ext.h>
#include <stdint.h>

typedef void sqlite3_t;
typedef void sqlite3_stmt_t;

/* from original sqlite3.h */
#define SQLITE_OK           (0)   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        (1)   /* Generic error */
#define SQLITE_INTERNAL     (2)   /* Internal logic error in SQLite */
#define SQLITE_PERM         (3)   /* Access permission denied */
#define SQLITE_ABORT        (4)   /* Callback routine requested an abort */
#define SQLITE_BUSY         (5)   /* The database file is locked */
#define SQLITE_LOCKED       (6)   /* A table in the database is locked */
#define SQLITE_NOMEM        (7)   /* A malloc() failed */
#define SQLITE_READONLY     (8)   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    (9)   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR        (10)   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT      (11)   /* The database disk image is malformed */
#define SQLITE_NOTFOUND     (12)   /* Unknown opcode in sqlite3_file_control() */
#define SQLITE_FULL         (13)   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN     (14)   /* Unable to open the database file */
#define SQLITE_PROTOCOL     (15)   /* Database lock protocol error */
#define SQLITE_EMPTY        (16)   /* Internal use only */
#define SQLITE_SCHEMA       (17)   /* The database schema changed */
#define SQLITE_TOOBIG       (18)   /* String or BLOB exceeds size limit */
#define SQLITE_CONSTRAINT   (19)   /* Abort due to constraint violation */
#define SQLITE_MISMATCH     (20)   /* Data type mismatch */
#define SQLITE_MISUSE       (21)   /* Library used incorrectly */
#define SQLITE_NOLFS        (22)   /* Uses OS features not supported on host */
#define SQLITE_AUTH         (23)   /* Authorization denied */
#define SQLITE_FORMAT       (24)   /* Not used */
#define SQLITE_RANGE        (25)   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB       (26)   /* File opened that is not a database file */
#define SQLITE_NOTICE       (27)   /* Notifications from sqlite3_log() */
#define SQLITE_WARNING      (28)   /* Warnings from sqlite3_log() */
#define SQLITE_ROW          (100)  /* sqlite3_step() has another row ready */
#define SQLITE_DONE         (101)  /* sqlite3_step() has finished executing */

/* column type */
#define SQLITE_INTEGER      (1)
#define SQLITE_FLOAT        (2)
#define SQLITE_TEXT         (3)
#define SQLITE_BLOB         (4)
#define SQLITE_NULL         (5)

/* no timeout used */
#define SQLITE_NO_TIMEOUT   (-1)

/* ext public functions */
extern sqlite3_t *sqlite3_open(const char *filename, int timeout);
extern int sqlite3_close(sqlite3_t *db);
extern const char *sqlite3_last_errmsg(sqlite3_t *db);
extern int sqlite3_exec(sqlite3_t *db, const char *sql);
extern sqlite3_stmt_t *sqlite3_prepare(sqlite3_t *db, const char *sql);
extern void sqlite3_finalize(sqlite3_stmt_t *stmt);
extern int sqlite3_bind_null(sqlite3_stmt_t *stmt, int index);
extern int sqlite3_bind_blob(sqlite3_stmt_t *stmt, int index, void *blob, int len, int mkcopy);
extern int sqlite3_bind_double(sqlite3_stmt_t *stmt, int index, double val);
extern int sqlite3_bind_int(sqlite3_stmt_t *stmt, int index, int val);
extern int sqlite3_bind_text(sqlite3_stmt_t *stmt, int index, const char *val, int len);
extern int sqlite3_step(sqlite3_stmt_t *stmt);
extern int sqlite3_clear_bindings(sqlite3_stmt_t *stmt);
extern int sqlite3_reset(sqlite3_stmt_t *stmt);
extern int sqlite3_column_count(sqlite3_stmt_t *stmt);
extern int sqlite3_column_type(sqlite3_stmt_t *stmt, int index);
extern int sqlite3_column_bytes(sqlite3_stmt_t *stmt, int index);
extern int64_t sqlite3_column_int(sqlite3_stmt_t *stmt, int index);
extern double sqlite3_column_double(sqlite3_stmt_t *stmt, int index);
extern void *sqlite3_column_blob(sqlite3_stmt_t *stmt, int index);
extern const char *sqlite3_column_text(sqlite3_stmt_t *stmt, int index);
extern const char *sqlite3_column_name(sqlite3_stmt_t *stmt, int index);

#ifndef KCC_NO_IMPORT
#if defined(__KCC_JIT__) || defined(__KCC__)
#include <../libsrc/kcc/ext_sqlite3.c>
#endif
#endif

#endif /* KCC_SQLITE3_H */
