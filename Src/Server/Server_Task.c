#include "Server/Server_Task.h"
#include "Client/Dispatcher_Task.h" // חייבים להכיר את xIncomingEventsQueue כדי לשלוח אליו

void vServerTask(void *pvParameters) {
    EmergencyEvent_t xNewEvent;
    uint32_t ulIDCounter = 1;

    printf("[Server] Task Started\n");

    for (;;) {
        // 1. יצירת אירוע אקראי
        xNewEvent.eventID = ulIDCounter++;
        xNewEvent.type = (EventType_t)(rand() % EVENT_MAX);
        xNewEvent.priority = (uint8_t)((rand() % 3) + 1);
        snprintf(xNewEvent.location, sizeof(xNewEvent.location), "Street %d", rand() % 100);
        xNewEvent.timestampStart = (uint32_t)xTaskGetTickCount();

        printf("[Server] Generated: ID %u, Type %d, Priority %d\n", 
                xNewEvent.eventID, xNewEvent.type, xNewEvent.priority);
        
        // 2. שליחה ללקוח דרך התור (כפי שמוגדר ב-Dispatcher_Task.h)
        if (xIncomingEventsQueue != NULL) {
            if (xQueueSend(xIncomingEventsQueue, &xNewEvent, pdMS_TO_TICKS(100)) == pdPASS) {
                printf("[Server] Event %u sent successfully to Dispatcher\n", xNewEvent.eventID);
            } else {
                printf("[Server Error] Could not send Event %u - Queue Full\n", xNewEvent.eventID);
            }
        } else {
            printf("[Server Error] xIncomingEventsQueue is NULL! Make sure vCreateGlobalResources ran.\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // אירוע כל 5 שניות
    }
}