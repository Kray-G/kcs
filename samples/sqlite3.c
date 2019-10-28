#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kcc/sqlite3.h>

void error(sqlite3_t *db, const char *msg)
{
    printf("%s: ", msg);
    const char *err = sqlite3_last_errmsg(db);
    if (err) {
        printf("%s\n", err);
    } else {
        printf("Unknown error\n");
    }
}

int main(void)
{
    sqlite3_t *db = NULL;
    sqlite3_stmt_t *stmt = NULL;

    db = sqlite3_open("sample.db", -1);
    if (!db) {
        error(db, "File open error");
        goto END;
    }

    int r = sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS SampleTable "
            "("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name CHAR(32) NOT NULL"
            ")"
    );
    if (r != SQLITE_OK) {
        error(db, "Create table error");
        goto END;
    }

    int rows = 0;
    stmt = sqlite3_prepare(db, "SELECT count(*) FROM SampleTable");
    if ((r = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *name = sqlite3_column_name(stmt, 0);
        rows = sqlite3_column_int(stmt, 0);
        printf("rows check: %s = %d\n", name, rows);
        sqlite3_finalize(stmt);
        stmt = NULL;
    }

    if (rows == 0) {
        r = sqlite3_exec(db, "INSERT INTO SampleTable(id,name) VALUES(10, 'aaa')");
        r = sqlite3_exec(db, "INSERT INTO SampleTable(id,name) VALUES(20, 'bbb')");
        r = sqlite3_exec(db, "INSERT INTO SampleTable(id,name) VALUES(30, 'ccc')");
        if (r != SQLITE_OK) {
            error(db, "Data insertion error");
            goto END;
        }
    }

    stmt = sqlite3_prepare(db, "SELECT * FROM SampleTable WHERE id > ?");
    if (!stmt) {
        error(db, "Create statement error");
        goto END;
    }

    r = sqlite3_bind_int(stmt, 1, 10);
    if (r != SQLITE_OK) {
        error(db, "Binding error");
        goto END;
    }

    int row = 0;
    while ((r = sqlite3_step(stmt)) == SQLITE_ROW) {
        printf("[row:%d]\n", ++row);
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; ++i) {
            int type = sqlite3_column_type(stmt, i);
            printf("    [col:%d] ", i);
            const char *name = sqlite3_column_name(stmt, i);
            switch (type) {
            case SQLITE_INTEGER:
                printf("%s = %d\n", name, sqlite3_column_int(stmt, i));
                break;
            case SQLITE_FLOAT:
                printf("%s = %f\n", name, sqlite3_column_double(stmt, i));
                break;
            case SQLITE_TEXT:
                printf("%s = %s\n", name, sqlite3_column_text(stmt, i));
                break;
            case SQLITE_BLOB:
                printf("%s = %p, len(%d)\n", name, sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i));
                break;
            case SQLITE_NULL:
                printf("%s = (NULL)\n", name);
                break;
            }
        }
    }
    if (r != SQLITE_DONE){
        error(db, "Reading data error");
        goto END;
    }

END:
    if (stmt) {
        sqlite3_finalize(stmt);
    }
    if (db) {
        sqlite3_close(db);
    }
    return 0;
}
