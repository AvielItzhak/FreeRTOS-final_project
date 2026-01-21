
/**
 * @file Database.c
 * @attention This module implements the SQLite database interface for emergency event logging.
 */

#include "Server/DataBase.h"
#include <stdint.h>
#include <stdio.h>

/* SQLite Configuration */

#ifndef SQLITE_DB_PATH 
#define SQLITE_DB_PATH "EventLog.db"
#endif // SQLITE_DB_PATH

/* Status code for pending events */
#ifndef STATUS_PENDING 
#define STATUS_PENDING 1
#endif

static sqlite3 *handle_db = NULL; // Global DB handle initialized to NULL
static SemaphoreHandle_t handle_dbMutex = NULL; // Mutex for thread-safe DB access

void Db_Init(void)
{
    if (handle_dbMutex == NULL) { // Create mutex if not already created
        handle_dbMutex = xSemaphoreCreateMutex();
        if (handle_dbMutex == NULL) { // Failed to create mutex
            printf("[DB] ERROR: failed to create DB mutex\n");
            return;
        }
    }

    if (handle_db != NULL) { // Already initialized
        return;
    }

    if (sqlite3_open(SQLITE_DB_PATH, &handle_db) != SQLITE_OK) { // Could not open DB
        printf("[DB] sqlite3_open failed: %s\n", sqlite3_errmsg(handle_db));
        handle_db = NULL; // Ensure handle_db is NULL on failure
        return;
    }

    /* Setting up the database schema */
    const char *sql =
        "CREATE TABLE IF NOT EXISTS events ("
        " event_id      INTEGER PRIMARY KEY,"
        " event_type    INTEGER NOT NULL,"
        " event_detail  TEXT,"
        " priority      INTEGER NOT NULL,"
        " location      TEXT    NOT NULL,"
        " ts_start      INTEGER NOT NULL,"
        " ts_end        INTEGER,"
        " handled_by    TEXT,"
        " status        INTEGER NOT NULL"
        ");";

    /* Execute the schema creation SQL */
    char *err = NULL; // Error message pointer
    int rc = sqlite3_exec(handle_db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) { // Schema creation failed
        printf("[DB] schema error: %s\n", err ? err : "unknown");
        sqlite3_free(err);
    } else { // Schema ready
        printf("[DB] ready: %s\n", SQLITE_DB_PATH);
    }
}

uint32_t Db_GetNextEventId(void)
{
    if (!handle_db || !handle_dbMutex) return 1; // DB not initialized

    uint32_t next = 1; // Default next ID
    sqlite3_stmt *stmt = NULL; // Prepared statement initialization

    xSemaphoreTake(handle_dbMutex, portMAX_DELAY); // Lock the mutex

    if (sqlite3_prepare_v2(handle_db,
                           "SELECT IFNULL(MAX(event_id), 0) FROM events;",
                           -1, &stmt, NULL) == SQLITE_OK) 
    { // Prepared successfully
        if (sqlite3_step(stmt) == SQLITE_ROW) { 
            uint32_t maxId = (uint32_t)sqlite3_column_int(stmt, 0); // Get max ID
            next = maxId + 1; // Next ID is max + 1
        }
    } else { // Preparation failed
        printf("[DB] max(id) prepare failed: %s\n", sqlite3_errmsg(handle_db));
    }

    if (stmt) sqlite3_finalize(stmt); // Check if stmt is not NULL before finalizing
    xSemaphoreGive(handle_dbMutex); // Unlock the mutex

    return next; // Return the next event ID
}

void Db_InsertEventPending(const EmergencyEvent_t *event)
{
    if (!handle_db || !handle_dbMutex || !event) return; // DB not initialized or invalid input

    /* Create the SQL statement for inserting an event */
    const char *sql =
    "INSERT INTO events(event_id, event_type, event_detail, priority, location, ts_start, ts_end, handled_by, status) "
    "VALUES(?,?,?,?,?,?,NULL,NULL,?);";

    sqlite3_stmt *stmt = NULL; // Prepared statement initialization

    xSemaphoreTake(handle_dbMutex, portMAX_DELAY); // Lock the mutex
    /* Prepare and execute the SQL statement */
    if (sqlite3_prepare_v2(handle_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)event->eventID);
        sqlite3_bind_int(stmt, 2, (int)event->type);
        sqlite3_bind_text(stmt, 3, event->event_detail, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, (int)event->priority);
        sqlite3_bind_text(stmt, 5, event->location, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 6, (int)event->timestampStart);
        sqlite3_bind_int(stmt, 7, STATUS_PENDING);

        if (sqlite3_step(stmt) != SQLITE_DONE) { // Execution failed
            printf("[DB] insert failed (id=%u): %s\n",
                   (unsigned)event->eventID, sqlite3_errmsg(handle_db));
        }
    } else { // Preparation failed
        printf("[DB] prepare failed: %s\n", sqlite3_errmsg(handle_db));
    }

    if (stmt) sqlite3_finalize(stmt); // Check if statement pointer is not NULL before finalizing
    xSemaphoreGive(handle_dbMutex); // Unlock the mutex
}

void Db_UpdateEventCompletion(const CompletionMsg_t *msg)
{
    if (!handle_db || !handle_dbMutex || !msg) return; // DB not initialized or invalid input

    /* Create the SQL statement for updating an event */
    const char *sql =
        "UPDATE events "
        "SET ts_end = ?, handled_by = ?, status = ? "
        "WHERE event_id = ?;";

    sqlite3_stmt *stmt = NULL; // Prepared statement initialization

    xSemaphoreTake(handle_dbMutex, portMAX_DELAY); // Lock the mutex

    /* Prepare and execute the SQL statement */
    if (sqlite3_prepare_v2(handle_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)msg->timestampEnd);
        sqlite3_bind_text(stmt, 2, msg->handledBy, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, (int)msg->status);
        sqlite3_bind_int(stmt, 4, (int)msg->eventID);

        if (sqlite3_step(stmt) != SQLITE_DONE) { // Execution failed
            printf("[DB] update failed (id=%u): %s\n",
                   (unsigned)msg->eventID, sqlite3_errmsg(handle_db));
        }
    } else { // Preparation failed
        printf("[DB] prepare failed: %s\n", sqlite3_errmsg(handle_db));
    }

    if (stmt) sqlite3_finalize(stmt); // Check if statement pointer is not NULL before finalizing
    xSemaphoreGive(handle_dbMutex); // Unlock the mutex
}

void Db_Close(void)
{
    if (!handle_db || !handle_dbMutex) return; // DB not initialized

    xSemaphoreTake(handle_dbMutex, portMAX_DELAY); // Lock the mutex
    sqlite3_close(handle_db); // Close the database
    handle_db = NULL; // Reset DB handle
    xSemaphoreGive(handle_dbMutex); // Unlock the mutex
}