/**
 * @file Vehicle_Task.c
 * @brief 
 * 
 * @attention
 */

#include "Client/Vehicle_Task.h"
#include "Shared_Configuration.h"

#include <stdio.h>


void Task_TestVehicle(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][VEHICLE] Started\n");

    /* Main loop for vehicle Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptAmbulanceQ, &event, portMAX_DELAY) == pdPASS) { // Using Ambulance queue for testing
            printf("[Client][VEHICLE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(1000)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Vehicle");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][VEHICLE] Drop completion id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][VEHICLE] Sent completion id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][VEHICLE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}