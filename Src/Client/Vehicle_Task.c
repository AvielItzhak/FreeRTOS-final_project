/**
 * @file Vehicle_Task.c
 * @brief This file contains the implementation of each department vehicle tasks that simulate communication between vehicles and the dispatcher.
 * 
 * @attention Task_TestVehicle is for testing purposes only - and will not be created in the final system.
 */

#include "Client/Vehicle_Task.h"
#include "Shared_Configuration.h"

#include <stdio.h>



void Task_TestVehicle(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][VEHICLE] Started\n");

    /* Main loop for Vehicle -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptAmbulanceQ, &event, portMAX_DELAY) == pdPASS) { // Using Ambulance queue for testing
            printf("[Client][VEHICLE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Vehicle");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][VEHICLE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][VEHICLE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][VEHICLE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Ambulance_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][AMBULANCE] Started\n");

    /* Main loop for Ambulance -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptAmbulanceQ, &event, portMAX_DELAY) == pdPASS) { // Using Ambulance queue for testing
            printf("[Client][AMBULANCE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Ambulance");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][AMBULANCE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][AMBULANCE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][AMBULANCE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Police_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][POLICE] Started\n");

    /* Main loop for Police -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptPoliceQ, &event, portMAX_DELAY) == pdPASS) {
            printf("[Client][POLICE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Police");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][POLICE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][POLICE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][POLICE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Fire_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][FIRE] Started\n");

    /* Main loop for Fire Department -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptFireQ, &event, portMAX_DELAY) == pdPASS) {
            printf("[Client][FIRE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "FireDept");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][FIRE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][FIRE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][FIRE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Maintenance_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][MAINTENANCE] Started\n");

    /* Main loop for Maintenance -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptMaintQ, &event, portMAX_DELAY) == pdPASS) {
            printf("[Client][MAINTENANCE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Maintenance");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][MAINTENANCE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][MAINTENANCE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][MAINTENANCE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Waste_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][WASTE] Started\n");

    /* Main loop for Waste Collection -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptWasteQ, &event, portMAX_DELAY) == pdPASS) {
            printf("[Client][WASTE] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "WasteCollect");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][WASTE] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][WASTE] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][WASTE] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Electricity_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][ELECTRICITY] Started\n");

    /* Main loop for Electricity Department -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptElectQ, &event, portMAX_DELAY) == pdPASS) {
            printf("[Client][ELECTRICITY] Handling event id=%u type=%d\n",
                   (unsigned)event.eventID, (int)event.type);

            /* Simulate handling the event (e.g., delay based on priority) */
            vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "Electricity");
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][ELECTRICITY] Drop completion message id=%u (TX queue full)\n", (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][ELECTRICITY] Sent completion message id=%u\n", (unsigned)Msg.eventID);
            }

            printf("[Client][ELECTRICITY] Completed event id=%u\n", (unsigned)event.eventID);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

