/**
 * @file Server_Task.c
 * @attention This module implements the server task that generates random emergency events
 *            and stores them in an SQLite database.
 */

#include "Server/Server_Task.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"

#include "Server/DataBase.h" // Database functions

void vServerEventGen(void *pvParameters)
{
    (void)pvParameters;

    Db_InitOnce();

    /* Seed rand() once so each RUN offers random results */
    static int seeded = 0; // Static variable to track if seeded
    if (!seeded) { // Only if Not seeded yet
        seeded = 1; // Mark as seeded
        srand((unsigned)time(NULL) ^ (unsigned)xTaskGetTickCount()); // Seed with time and tick count
    }

    uint32_t ulIDCounter = Db_GetNextEventId(); // Get starting event ID from DB

    printf("[Server] EventGen Started (next_id=%u)\n", (unsigned)ulIDCounter);

    /* Main Loop to Generate events and insert them into the database */
    for (;;) {
        EmergencyEvent_t xNewEvent; // New event structure

        xNewEvent.eventID  = ulIDCounter++; // Assign and increment event ID
        xNewEvent.type     = (EventType_t)(rand() % EVENT_MAX); // Random event type with the range of 1 to EVENT_MAX -1
        xNewEvent.priority = (uint8_t)((rand() % MAX_PRIORITY_LEVEL) + 1U); // Random priority between 1 and MAX_PRIORITY_LEVEL

        /* Generate a random location using snprintf */
        snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                 "Street %u", (unsigned)(rand() % 100U)); // Random street number between 0 and 99

        TickType_t now = xTaskGetTickCount(); // Get current tick count
        xNewEvent.timestampStart = (uint32_t)now; // Set start timestamp

        Db_InsertEventPending(&xNewEvent); // Insert the new event into the database

        /* Print the generated event details */
        printf("[Server] Generated: ID=%u Type=%d Priority=%u Location='%s' Time=%lusec\n",
               (unsigned)xNewEvent.eventID,
               (int)xNewEvent.type,
               (unsigned)xNewEvent.priority,
               xNewEvent.location,
               (unsigned long)(now / 1000U));

        vTaskDelay(pdMS_TO_TICKS(EVENT_GENERATION_INTERVAL_MS)); // Delay before generating the next event
    }
}
