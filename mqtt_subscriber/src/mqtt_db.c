#include "mqtt_db.h"
/**
 * checking if database table exists, if not creates one
 * @return: 0 || 1 - sucess, other - sqlite3 problems (see sqlite3 documentation)
 */
static int create_table_if_not_exist()
{
    sqlite3_stmt *stmt = NULL;
    int rc = 0;
    char sql[100];

    sprintf(sql, "SELECT COUNT(TYPE) FROM sqlite_master WHERE TYPE='table' AND NAME='%s';", DB_TABLE);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
            fprintf(stderr, "error preparing sql statement\n");
            goto cleanup;
    }

    sqlite3_step(stmt);
    rc = sqlite3_column_int(stmt, 0);

    if (rc == SQLITE_OK) {
            rc = create_table();
            if (rc != SQLITE_OK) {
                    fprintf(stderr, "creating table failed\n");
                    goto cleanup;
            }
    }

    cleanup:
            sqlite3_finalize(stmt);
            return rc;
}

/**
 * adds messages to database
 * @return: 0 - success, -1 - allocation problems, other - sqlite3 problems (see sqlite3 documentation)
 */
static int create_table()
{
    char *err_msg = NULL;
    int rc = 0;
    char sql[120];
    
    sprintf(sql, "CREATE TABLE '%s' (topic varchar(255), payload varchar(255), time varchar(100));", DB_TABLE);

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error while creating table: %s\n", err_msg);
            sqlite3_free(err_msg);
            
            return rc;
    }
    return rc;
}
/**
 * Open a connection to SQLite database (creates new if not exist)
 * @return: 0 - success, 1 - failed to open database
 */
extern int open_db()
{
    int rc = 0;

    rc = sqlite3_open(DB_FILE, &db);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "could not open database file: %s\n", sqlite3_errmsg(db));
            return 1;
    }
    return 0;
}
/**
 * close connection to database
 * @return: 0 - success, 1 - failed to open database
 */
extern int close_db()
{
    if (sqlite3_close(db) != SQLITE_OK) {
            db = NULL;
            return 1;
    }
    return 0;
}
/**
 * adds messages to database
 * @return: 0 - success, other - sqlite3 problems (see sqlite3 documentation)
 */
extern int add_message_to_db(char *topic, char *payload)
{
    char *sql_message = NULL;
    char *err_msg = NULL;
    char *date = NULL;
    int rc = 0;

    rc = create_table_if_not_exist();

    if (rc != 1 && rc != 0)
            goto cleanup;

    date = time_stamp();

    sql_message = sqlite3_mprintf("INSERT INTO %s VALUES ('%q', '%q', '%q');", DB_TABLE, topic, payload, date);

    rc = sqlite3_exec(db, sql_message, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error while adding message to DB: %s\n", err_msg);
            sqlite3_free(err_msg);
    }

    sqlite3_free(sql_message);
    cleanup:
            free(date);
            return rc;
}

/**
 * return your computer date with time
 * @return: date time
 */
static char *time_stamp()
{
    time_t rawtime;
    struct tm *timeinfo;
    char *date;
    date = (char *) malloc(sizeof(char) * 35);

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strcpy(date, asctime(timeinfo));

    date[strcspn(date, "\n")] = 0; //!< used for removing \n symbol in the end

    return date;
}