/**
 * @file Vehicle_Task.c
 * @brief This file contains the implementation of each department vehicle tasks that simulate communication between vehicles and the dispatcher.
 * 
 * @attention Task_TestVehicle is for testing purposes only - and will not be created in the final system.
 */

#include "Client/Vehicle_Task.h"
#include "Shared_Configuration.h"

#include <stdio.h>

// Helper function to map EventType_t to corresponding department counting semaphore handle
static SemaphoreHandle_t DeptSemFromType(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:        return handleSem_deptAmbulance;
        case EVENT_POLICE:           return handleSem_deptPolice;
        case EVENT_FIRE_DEPARTMENT:  return handleSem_deptFire;
        case EVENT_MAINTENANCE:      return handleSem_deptMaint;
        case EVENT_WASTE_COLLECTION: return handleSem_deptWaste;
        case EVENT_ELECTRICITY:      return handleSem_deptElect;
        default:                     return NULL;
    }
}



void Task_TestVehicle(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][VEHICLE] Started\n");

    /* Main loop for Vehicle -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptAmbulanceQ, &event, portMAX_DELAY) == pdPASS) { // Using Ambulance queue for testing
            printf("[Client][VEHICLE] Handling event id=%u type=%d priority=%d\n",
                   (unsigned)event.eventID, (int)event.type, (int)event.priority);

            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

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

    printf("[Client][%s] Started\n",pcTaskGetName(NULL));

    /* Main loop for Ambulance -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptAmbulanceQ, &event, portMAX_DELAY) == pdPASS) { // Using Ambulance queue for testing

            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptAmbulance

            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)
            
            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority);

            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }
        
    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Police_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));

    /* Main loop for Police -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptPoliceQ, &event, portMAX_DELAY) == pdPASS) {
            
            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptPolice
            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)
            
            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority); 
            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }

    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Fire_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));

    /* Main loop for Fire Department -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptFireQ, &event, portMAX_DELAY) == pdPASS) {
            
            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptFire

            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)

            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority); 
            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }

    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Maintenance_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));

    /* Main loop for Maintenance -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptMaintQ, &event, portMAX_DELAY) == pdPASS) {
            
            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptMaint

            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)

            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority);
            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }

    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Waste_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));

    /* Main loop for Waste Collection -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptWasteQ, &event, portMAX_DELAY) == pdPASS) {

            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptWaste

            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)

            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority);
            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }

    }

    vTaskDelete(NULL); // Should never reach here
}

void Task_Electricity_X(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));

    /* Main loop for Electricity Department -> Dispatcher communication */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the Dispatcher queue */
        if (xQueueReceive(handle_deptElectQ, &event, portMAX_DELAY) == pdPASS) {
            
            SemaphoreHandle_t countSemaphore = DeptSemFromType(event.type); // Returns handleSem_deptElect

            /* Check if the semaphore handle is valid */
            if (countSemaphore == NULL) {
                printf("[Client][%s] ERROR: Semaphore handle is NULL\n", pcTaskGetName(NULL));
                continue;
            }
            xSemaphoreTake(countSemaphore, portMAX_DELAY); // blocks if no vehicles available (counting semaphore)

            printf("[Client][%s] Handling event id=%u type=%d priority=%d\n",
                   pcTaskGetName(NULL), (unsigned)event.eventID, (int)event.type, (int)event.priority);
            /* Simulate handling the event - Very long delay */
            vTaskDelay(pdMS_TO_TICKS(baseEventHandling_Delay_MS * event.delayFactor)); // Simulated handling time

            /* Prepare completion message */
            CompletionMsg_t Msg;
            Msg.eventID   = event.eventID;
            Msg.status    = 0; // Success
            snprintf(Msg.handledBy, sizeof(Msg.handledBy), "%s", pcTaskGetName(NULL));
            Msg.timestampEnd = xTaskGetTickCount(); // Current tick count as end timestamp

            /* Send completion message to Client UDP TX queue */
            BaseType_t queueCheck = xQueueSend(handle_clientUDPTxQ, &Msg, 0);
            if (queueCheck != pdPASS) {
                printf("[Client][%s] Drop completion message id=%u (TX queue full)\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }
            else {
                printf("[Client][%s] Sent completion message id=%u\n", pcTaskGetName(NULL), (unsigned)Msg.eventID);
            }

            printf("[Client][%s] Completed event id=%u\n", pcTaskGetName(NULL), (unsigned)event.eventID);

            xSemaphoreGive(countSemaphore); // Release vehicle resource back
        }

    }

    vTaskDelete(NULL); // Should never reach here
}

