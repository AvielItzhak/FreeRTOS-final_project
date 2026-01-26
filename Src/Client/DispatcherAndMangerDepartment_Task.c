/**
 * @file Dispatcher_Task.c
 * @brief Receives events from UDP RX queue and forwards them to the appropriate department queue based on event type.
 * 
 * @attention Static functions are used to map event types to their respective queues, semaphores and mutexes and only used within this file.
 */

#include "Client/DispatcherAndMangerDepartment_Task.h"
#include "Shared_Configuration.h"

#include <stdio.h>


/**
 * @brief Maps EventType_t to corresponding department queue handle using switch case function.
 *        Been Called only within this file and used in Dispatcher task to forward events.
 * @param type Given event type of EventType_t enum.
 * 
 * @attention This function is static and only used within this file.
 * @return QueueHandle_t Corresponding department queue handle, or NULL if invalid type.
 */
static QueueHandle_t DeptQueueFromType(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:           return handle_deptAmbulanceQ;
        case EVENT_POLICE:              return handle_deptPoliceQ;
        case EVENT_FIRE_DEPARTMENT:     return handle_deptFireQ;
        case EVENT_MAINTENANCE:         return handle_deptMaintQ;
        case EVENT_WASTE_COLLECTION:    return handle_deptWasteQ;
        case EVENT_ELECTRICITY:         return handle_deptElectQ;
        default:                        return NULL; // Invalid type
    }
}

/**
 * @brief Maps EventType_t to corresponding department mutex handle using switch case function.
 *        Been Called only within this file and used in Dispatcher task to protect department queues.
 * @param type Given event type of EventType_t enum.
 * 
 * @attention This function is static and only used within this file.
 * @return SemaphoreHandle_t Corresponding department mutex handle, or NULL if invalid type.
 */
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

/**
 * @brief Maps EventType_t to corresponding department counting semaphore handle using switch case function.
 *        Been Called only within this file and can be used in Dispatcher or Vehicle tasks to manage vehicle availability.
 * @param type Given event type of EventType_t enum.
 * 
 * @attention This function is static and only used within this file.
 * @return SemaphoreHandle_t Corresponding department counting semaphore handle, or NULL if invalid type.
 */
static SemaphoreHandle_t DeptVehicleSemFromType(EventType_t type)
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


static SemaphoreHandle_t DeptAvailSemFromType(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:           return handleSem_deptAmbulance;
        case EVENT_POLICE:              return handleSem_deptPolice;
        case EVENT_FIRE_DEPARTMENT:     return handleSem_deptFire;
        case EVENT_MAINTENANCE:         return handleSem_deptMaint;
        case EVENT_WASTE_COLLECTION:    return handleSem_deptWaste;
        case EVENT_ELECTRICITY:         return handleSem_deptElect;
        default:                        return NULL;
    }
}

/* ---------- INIT ---------- */

void ClientDeptManager_Init(DepartmentDescription_t *pvParameters)
{
    DepartmentDescription_t *dept = pvParameters;
    for (size_t i = 0; i < EVENT_MAX; i++) {
        dept[i].queue       = DeptQueueFromType(dept[i].type);
        dept[i].mutex   = DeptMutexFromType(dept[i].type);
        dept[i].availableSem = DeptAvailSemFromType(dept[i].type);
    }

    /* Seed tokens = vehicles available initially */
    for (int i=0; i<AMBULANCE_VEHICLES; i++) xSemaphoreGive(handleSem_deptAmbulance);
    for (int i=0; i<POLICE_VEHICLES; i++)    xSemaphoreGive(handleSem_deptPolice);
    for (int i=0; i<FIRE_VEHICLES; i++)      xSemaphoreGive(handleSem_deptFire);
    for (int i=0; i<MAINTENANCE_VEHICLES; i++)     xSemaphoreGive(handleSem_deptMaint);
    for (int i=0; i<WASTE_VEHICLES; i++)     xSemaphoreGive(handleSem_deptWaste);
    for (int i=0; i<ELECTRICITY_VEHICLES; i++)     xSemaphoreGive(handleSem_deptElect);

    printf("[Client][ALL MANAGERS] Department array initialized and semaphores seeded\n");
}

/* ---------- TASKS ---------- */

/* Main Task to handle Dispatcher responsibilities */
void Task_Dispatcher(void *pvParameters)
{
    (void)pvParameters;
    
    printf("[Client][DISPATCHER] Started\n");
    

    for (;;) {
        EmergencyEvent_t event;

        /* Wait for incoming event from UDP-RX queue */
        if (xQueueReceive(handle_clientUDPRxQ, &event, portMAX_DELAY) == pdPASS) {

            QueueHandle_t queue = DeptQueueFromType(event.type); // Get corresponding department queue
            SemaphoreHandle_t mutex = DeptMutexFromType(event.type); // Get corresponding department mutex

            if (queue == NULL || mutex == NULL ) { // Invalid event type
                printf("[Client][DISPATCHER] Invalid event type=%d received, discarding\n", (int)event.type);
                continue; // Skip invalid event
            }

            /* Mutex per department queue */
            xSemaphoreTake(mutex, portMAX_DELAY); // Wait indefinitely for mutex
            (void)xQueueSend(queue, &event, 0);
            xSemaphoreGive(mutex);

            printf("[Client][DISPATCHER] Forwarded id=%u type=%d priority=%u\n",
                   (unsigned)event.eventID, (int)event.type, (unsigned)event.priority);
        }
    }

    vTaskDelete(NULL); // Should never reach here
}

// TODO: Implement Task_Manager_X which monitors department queues and vehicle availability and controls dispatching logic.
      // After there is a need to implement sub prioritization or advanced dispatching strategies.

void Task_Manager_Departments_X(void *pvParameters)
{
    DepartmentDescription_t *dept = (DepartmentDescription_t *)pvParameters;

    if (dept == NULL ||
        dept->queue == NULL ||
        dept->mutex == NULL ||
        dept->availableSem == NULL)
    {
        printf("[Client][%s] ERROR: bad pvParameters / NULL handles\n", pcTaskGetName(NULL));
        vTaskDelete(NULL);
    }

    printf("[Client][%s] Started\n", pcTaskGetName(NULL));


    for (;;) {
        BaseType_t didWork = pdFALSE;

        if (uxQueueMessagesWaiting(dept->queue) > 0) {

            if (xSemaphoreTake(dept->availableSem, 0) == pdPASS) {

                EmergencyEvent_t event;

                xSemaphoreTake(dept->mutex, portMAX_DELAY);
                BaseType_t ok = xQueueReceive(dept->queue, &event, 0);
                xSemaphoreGive(dept->mutex);

                if (ok == pdPASS) {
                    didWork = pdTRUE;
                    printf("[Client][%s] Dept=%s took event id=%u\n",
                           pcTaskGetName(NULL), dept->name, (unsigned)event.eventID);

                    /* TODO: send event to actual vehicle (queue/notify). */
                } else {
                    /* couldn't pop -> return token */
                    xSemaphoreGive(dept->availableSem);
                }
            }
        }

        if (!didWork) vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL); // Should never reach here
}