
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

static sqlite3 *g_db = NULL; // Global DB handle initialized to NULL
static SemaphoreHandle_t g_dbMutex = NULL; // Mutex for thread-safe DB access

void Db_InitOnce(void)
{
    if (g_dbMutex == NULL) { // Create mutex if not already created
        g_dbMutex = xSemaphoreCreateMutex();
        if (g_dbMutex == NULL) { // Failed to create mutex
            printf("[DB] ERROR: failed to create DB mutex\n");
            return;
        }
    }

    if (g_db != NULL) { // Already initialized
        return;
    }

    if (sqlite3_open(SQLITE_DB_PATH, &g_db) != SQLITE_OK) { // Could not open DB
        printf("[DB] sqlite3_open failed: %s\n", sqlite3_errmsg(g_db));
        g_db = NULL; // Ensure g_db is NULL on failure
        return;
    }

    /* Setting up the database schema */
    const char *sql =
        "CREATE TABLE IF NOT EXISTS events ("
        " event_id     INTEGER PRIMARY KEY,"
        " event_type   INTEGER NOT NULL,"
        " priority     INTEGER NOT NULL,"
        " location     TEXT    NOT NULL,"
        " ts_start     INTEGER NOT NULL,"
        " ts_end       INTEGER,"
        " handled_by   INTEGER,"
        " status       INTEGER NOT NULL"
        ");";

    /* Execute the schema creation SQL */
    char *err = NULL; // Error message pointer
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) { // Schema creation failed
        printf("[DB] schema error: %s\n", err ? err : "unknown");
        sqlite3_free(err);
    } else { // Schema ready
        printf("[DB] ready: %s\n", SQLITE_DB_PATH);
    }
}

uint32_t Db_GetNextEventId(void)
{
    if (!g_db || !g_dbMutex) return 1; // DB not initialized

    uint32_t next = 1; // Default next ID
    sqlite3_stmt *stmt = NULL; // Prepared statement initialization

    xSemaphoreTake(g_dbMutex, portMAX_DELAY); // Lock the mutex

    if (sqlite3_prepare_v2(g_db,
                           "SELECT IFNULL(MAX(event_id), 0) FROM events;",
                           -1, &stmt, NULL) == SQLITE_OK) 
    { // Prepared successfully
        if (sqlite3_step(stmt) == SQLITE_ROW) { 
            uint32_t maxId = (uint32_t)sqlite3_column_int(stmt, 0); // Get max ID
            next = maxId + 1; // Next ID is max + 1
        }
    } else { // Preparation failed
        printf("[DB] max(id) prepare failed: %s\n", sqlite3_errmsg(g_db));
    }

    if (stmt) sqlite3_finalize(stmt); // Check if stmt is not NULL before finalizing
    xSemaphoreGive(g_dbMutex); // Unlock the mutex

    return next; // Return the next event ID
}

void Db_InsertEventPending(const EmergencyEvent_t *e)
{
    if (!g_db || !g_dbMutex || !e) return; // DB not initialized or invalid input

    /* Create the SQL statement for inserting an event */
    const char *sql =
        "INSERT INTO events(event_id, event_type, priority, location, ts_start, ts_end, handled_by, status) "
        "VALUES(?,?,?,?,?,NULL,NULL,?);";

    sqlite3_stmt *stmt = NULL; // Prepared statement initialization

    xSemaphoreTake(g_dbMutex, portMAX_DELAY); // Lock the mutex

    /* Prepare and execute the SQL statement */
    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)e->eventID);
        sqlite3_bind_int(stmt, 2, (int)e->type);
        sqlite3_bind_int(stmt, 3, (int)e->priority);
        sqlite3_bind_text(stmt, 4, e->location, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, (int)e->timestampStart);
        sqlite3_bind_int(stmt, 6, STATUS_PENDING);

        if (sqlite3_step(stmt) != SQLITE_DONE) { // Execution failed
            printf("[DB] insert failed (id=%u): %s\n",
                   (unsigned)e->eventID, sqlite3_errmsg(g_db));
        }
    } else { // Preparation failed
        printf("[DB] prepare failed: %s\n", sqlite3_errmsg(g_db));
    }

    if (stmt) sqlite3_finalize(stmt); // Check if statement pointer is not NULL before finalizing
    xSemaphoreGive(g_dbMutex); // Unlock the mutex
}

void Db_Close(void)
{
    if (!g_db || !g_dbMutex) return; // DB not initialized

    xSemaphoreTake(g_dbMutex, portMAX_DELAY); // Lock the mutex
    sqlite3_close(g_db); // Close the database
    g_db = NULL; // Reset DB handle
    xSemaphoreGive(g_dbMutex); // Unlock the mutex
}