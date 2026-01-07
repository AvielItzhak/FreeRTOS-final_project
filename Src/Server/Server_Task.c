/**
 * @file Server_Task.c
 * @brief this file contains the implementation of the Server tasks with the following functions:
 * - vServerEventGen: Generates random emergency events every 5 seconds.
 *
 *  
 */

#include "Server/Server_Task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// ServerEventGen generates random emergency events every 5 seconds
void vServerEventGen(void *pvParameters)
{
    (void)pvParameters;

    EmergencyEvent_t xNewEvent;
    uint32_t ulIDCounter = 1; // Event ID counter starting from 1

    printf("[Server] EventGen Started\n");

    for (;;)
    {
        // Generate random event data
        xNewEvent.eventID = ulIDCounter++;
        xNewEvent.type = (EventType_t)(rand() % EVENT_MAX);
        xNewEvent.priority = (uint8_t)((rand() % 3U) + 1U);

        // Generate random location and ensure null-termination
        snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                       "Street %u", (unsigned)(rand() % 100U));

        // Get current tick count and set as event start time
        TickType_t now = xTaskGetTickCount();
        xNewEvent.timestampStart = (uint32_t)now; 

        // Print generated event details
        printf("[Server] Generated: ID=%u Type=%d Priority=%u Location= %s Time= %lu sec\n",
               xNewEvent.eventID,
               (int)xNewEvent.type,
               (unsigned)xNewEvent.priority,
               xNewEvent.location,
               (unsigned long)now/1000U );/* convert to seconds */

        // 5 seconds delay before generating next event
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    // Should never reach here! - print and delete task
    printf("[Server] EventGen: Loop exited unexpectedly...Deleting Task\n");
    vTaskDelete(NULL);
}
