/**
 * @file Server_Task.c
 * @brief Server task: generates random emergency events and stores them in SQLite.
 */

#include "Server/Server_Task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <sqlite3.h>


#ifndef SQLITE_DB_PATH
#define SQLITE_DB_PATH "EventLog.db"
#endif

#ifndef STATUS_PENDING
#define STATUS_PENDING 1
#endif

static sqlite3 *g_db = NULL;
static SemaphoreHandle_t g_dbMutex = NULL;

/* Forward decls */
static void Db_InitOnce(void);
static uint32_t Db_GetNextEventId(void);
static void Db_InsertEventPending(const EmergencyEvent_t *e);

static void Db_InitOnce(void)
{
    if (g_dbMutex == NULL) {
        g_dbMutex = xSemaphoreCreateMutex();
        if (g_dbMutex == NULL) {
            printf("[Server][DB] ERROR: failed to create DB mutex\n");
            return;
        }
    }

    if (g_db != NULL) {
        return; // already initialized
    }

    if (sqlite3_open(SQLITE_DB_PATH, &g_db) != SQLITE_OK) {
        printf("[Server][DB] sqlite3_open failed: %s\n", sqlite3_errmsg(g_db));
        g_db = NULL;
        return;
    }

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

    char *err = NULL;
    int rc = sqlite3_exec(g_db, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        printf("[Server][DB] schema error: %s\n", err ? err : "unknown");
        sqlite3_free(err);
    } else {
        printf("[Server][DB] DB ready: %s\n", SQLITE_DB_PATH);
    }
}

/* Read MAX(event_id) so we continue IDs across program runs */
static uint32_t Db_GetNextEventId(void)
{
    if (!g_db || !g_dbMutex) return 1;

    uint32_t next = 1;
    sqlite3_stmt *stmt = NULL;

    xSemaphoreTake(g_dbMutex, portMAX_DELAY);

    if (sqlite3_prepare_v2(g_db,
                           "SELECT IFNULL(MAX(event_id), 0) FROM events;",
                           -1, &stmt, NULL) == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            uint32_t maxId = (uint32_t)sqlite3_column_int(stmt, 0);
            next = maxId + 1;
        }
    }
    else {
        printf("[Server][DB] max(id) prepare failed: %s\n", sqlite3_errmsg(g_db));
    }

    if (stmt) sqlite3_finalize(stmt);
    xSemaphoreGive(g_dbMutex);

    return next;
}

static void Db_InsertEventPending(const EmergencyEvent_t *e)
{
    if (!g_db || !g_dbMutex || !e) return;

    const char *sql =
        "INSERT INTO events(event_id, event_type, priority, location, ts_start, ts_end, handled_by, status) "
        "VALUES(?,?,?,?,?,NULL,NULL,?);";

    sqlite3_stmt *stmt = NULL;

    xSemaphoreTake(g_dbMutex, portMAX_DELAY);

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)e->eventID);
        sqlite3_bind_int(stmt, 2, (int)e->type);
        sqlite3_bind_int(stmt, 3, (int)e->priority);
        sqlite3_bind_text(stmt, 4, e->location, -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, (int)e->timestampStart);
        sqlite3_bind_int(stmt, 6, STATUS_PENDING);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("[Server][DB] insert failed (id=%u): %s\n",
                   (unsigned)e->eventID, sqlite3_errmsg(g_db));
        }
    } else {
        printf("[Server][DB] prepare failed: %s\n", sqlite3_errmsg(g_db));
    }

    if (stmt) sqlite3_finalize(stmt);
    xSemaphoreGive(g_dbMutex);
}

/* ServerEventGen generates random emergency events every 5 seconds */
void vServerEventGen(void *pvParameters)
{
    (void)pvParameters;

    Db_InitOnce();

    /* Seed rand() ONCE so each run differs */
    static int seeded = 0;
    if (!seeded) {
        seeded = 1;
        srand((unsigned)time(NULL) ^ (unsigned)xTaskGetTickCount());
    }

    /* Continue IDs from DB (prevents UNIQUE constraint across runs) */
    uint32_t ulIDCounter = Db_GetNextEventId();

    printf("[Server] EventGen Started (next_id=%u)\n", (unsigned)ulIDCounter);

    /* Event generation loop */
    for (;;) { 
        EmergencyEvent_t xNewEvent;

        xNewEvent.eventID  = ulIDCounter++;
        xNewEvent.type     = (EventType_t)(rand() % EVENT_MAX);
        xNewEvent.priority = (uint8_t)((rand() % MAX_PRIORITY_LEVEL) + 1U); // 1 to 3

        snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                 "Street %u", (unsigned)(rand() % 100U));

        TickType_t now = xTaskGetTickCount();
        xNewEvent.timestampStart = (uint32_t)now;

        Db_InsertEventPending(&xNewEvent); // Store in DB

        /* Log to console */
        printf("[Server] Generated: ID=%u Type=%d Priority=%u Location='%s' Time=%lusec\n",
               (unsigned)xNewEvent.eventID,
               (int)xNewEvent.type,
               (unsigned)xNewEvent.priority,
               xNewEvent.location,
               (unsigned long)(now / 1000U));

        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds delay between events generation
    }
}
