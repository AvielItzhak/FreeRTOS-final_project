/**
 * @file Dispatcher_Task.c
 * @brief Client Dispatcher Task: receives events from UDP RX queue and forwards them to the appropriate department queue.
 * 
 * @attention Static functions are used to map event types to their respective queues and mutexes and only used within this file.
 */

#include "Client/Dispatcher_Task.h"
#include "Shared_Configuration.h"

#include <stdio.h>

static QueueHandle_t DeptQueueFromType(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:       return handle_deptAmbulanceQ;
        case EVENT_POLICE:          return handle_deptPoliceQ;
        case EVENT_FIRE_DEPARTMENT: return handle_deptFireQ;
        case EVENT_MAINTENANCE:     return handle_deptMaintQ;
        case EVENT_WASTE_COLLECTION: return handle_deptWasteQ;
        case EVENT_ELECTRICITY:     return handle_deptElectQ;
        default:                    return NULL; // Invalid type
    }
}

static SemaphoreHandle_t DeptMutexFromType(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:        return handleMux_deptAmbulanceQ;
        case EVENT_POLICE:           return handleMux_deptPoliceQ;
        case EVENT_FIRE_DEPARTMENT:  return handleMux_deptFireQ;
        case EVENT_MAINTENANCE:      return handleMux_deptMaintQ;
        case EVENT_WASTE_COLLECTION: return handleMux_deptWasteQ;
        case EVENT_ELECTRICITY:      return handleMux_deptElectQ;
        default:                     return NULL; // Invalid type
    }
}

void Task_Dispatcher(void *pvParameters)
{
    (void)pvParameters;
    
    printf("[Client][DISPATCHER] Started\n");
    

    for (;;) {
        EmergencyEvent_t event;

        /* Wait for incoming event from UDP-RX queue */
        if (xQueueReceive(handle_clientUDPRxQ, &event, portMAX_DELAY) == pdPASS) {

            QueueHandle_t queue = DeptQueueFromType(event.type);
            SemaphoreHandle_t mutex = DeptMutexFromType(event.type);

            if (queue == NULL || mutex == NULL) { // Invalid event type
                printf("[Client][DISPATCHER] Invalid event type=%d received, discarding\n", (int)event.type);
                continue; // Skip invalid event
            }

            /* Mutex per department queue */
            xSemaphoreTake(mutex, portMAX_DELAY);
            (void)xQueueSend(queue, &event, 0);
            xSemaphoreGive(mutex);

            printf("[Client][DISPATCHER] Forwarded id=%u type=%d priority=%u\n",
                   (unsigned)event.eventID, (int)event.type, (unsigned)event.priority);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}