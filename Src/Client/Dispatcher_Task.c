
#include "Client/Dispatcher_Task.h"

QueueHandle_t xDeptQueues[EVENT_MAX];
QueueHandle_t xIncomingEventsQueue;


void vDispatcherTask(void *pvParameters) {
    EmergencyEvent_t xReceivedEvent;
    
    for (;;) {
        // המתנה לאירוע מהשרת
        if (xQueueReceive(xIncomingEventsQueue, &xReceivedEvent, portMAX_DELAY)) {
            // ניתוב למחלקה הרלוונטית
            xQueueSend(xDeptQueues[xReceivedEvent.type], &xReceivedEvent, 0);
        }
    }
}