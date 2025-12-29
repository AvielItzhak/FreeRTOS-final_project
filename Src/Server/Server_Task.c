/**
 * @file Server_Task.c
 * @brief Server task: periodically generates random emergency events and prints them.
 *
 * Notes:
 * - This project uses the POSIX/GCC FreeRTOS port, where TickType_t size may differ (32/64-bit).
 * - Using the wrong printf format specifier with variadic arguments can corrupt the stack
 *   and cause "*** stack smashing detected ***".
 * - Therefore: print TickType_t via a safe cast (unsigned long) and print uint32_t via PRIu32.
 */

#include "Server/Server_Task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>   /* PRIu32 */

#include "FreeRTOS.h"
#include "task.h"

/* Generate one random event and print it. */
void vServerTask(void *pvParameters)
{
    (void)pvParameters;

    EmergencyEvent_t xNewEvent;
    uint32_t ulIDCounter = 1;

    printf("[Server] Task Started\n");

    for (;;)
    {
        /* 1) Create a random event */
        xNewEvent.eventID = ulIDCounter++;
        xNewEvent.type = (EventType_t)(rand() % EVENT_MAX);
        xNewEvent.priority = (uint8_t)((rand() % 3U) + 1U);

        /* Ensure the string is always NULL-terminated */
        (void)snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                       "Street %u", (unsigned)(rand() % 100U));

        /* Capture tick count once */
        TickType_t now = xTaskGetTickCount();

        /* Store tick in the struct (struct uses uint32_t; explicit cast is intentional) */
        xNewEvent.timestampStart = (uint32_t)now;

        /* 2) Print safely:
              - uint32_t -> PRIu32
              - TickType_t -> cast to unsigned long and print with %lu
         */
        printf("[Server] Generated: ID=%" PRIu32 " Type=%d Priority=%u Location=%s Tick=%lu\n",
               xNewEvent.eventID,
               (int)xNewEvent.type,
               (unsigned)xNewEvent.priority,
               xNewEvent.location,
               (unsigned long)now);

        /* 3) Delay 5 seconds */
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
