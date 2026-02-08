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

static UBaseType_t DeptMaxVehicles(EventType_t type)
{
    switch (type) {
        case EVENT_AMBULANCE:        return AMBULANCE_VEHICLES;
        case EVENT_POLICE:           return POLICE_VEHICLES;
        case EVENT_FIRE_DEPARTMENT:  return FIRE_VEHICLES;
        case EVENT_MAINTENANCE:      return MAINTENANCE_VEHICLES;
        case EVENT_WASTE_COLLECTION: return WASTE_VEHICLES;
        case EVENT_ELECTRICITY:      return ELECTRICITY_VEHICLES;
        default:                     return 1;
    }
}

/* Cancel ONE lowest-priority event currently waiting in the department queue.
   Keeps all other events (stable), and sends a "cancelled" completion.
*/
static BaseType_t CancelOneLowestPriorityEvent(const DepartmentDescription_t *d)
{
    EmergencyEvent_t buf[DEPT_Q_LEN];
    UBaseType_t n = 0;

    xSemaphoreTake(d->mutex, portMAX_DELAY);

    n = uxQueueMessagesWaiting(d->queue);
    if (n == 0) {
        xSemaphoreGive(d->mutex);
        return pdFALSE;
    }
    if (n > DEPT_Q_LEN) n = DEPT_Q_LEN;

    /* Drain */
    for (UBaseType_t i = 0; i < n; i++) {
        if (xQueueReceive(d->queue, &buf[i], 0) != pdPASS) {
            n = i;
            break;
        }
    }

    /* Find lowest priority (priority: 1 low, 3 high) */
    UBaseType_t cancelIdx = 0;
    uint8_t minPrio = buf[0].priority;
    for (UBaseType_t i = 1; i < n; i++) {
        if (buf[i].priority < minPrio) {
            minPrio = buf[i].priority;
            cancelIdx = i;
        }
    }

    EmergencyEvent_t cancelled = buf[cancelIdx];

    /* Requeue everything except cancelled */
    for (UBaseType_t i = 0; i < n; i++) {
        if (i == cancelIdx) continue;
        (void)xQueueSend(d->queue, &buf[i], 0);
    }

    xSemaphoreGive(d->mutex);

    /* Notify server: event was cancelled */
    CompletionMsg_t msg = {0};
    msg.eventID = cancelled.eventID;
    msg.status  = STATUS_CANCELLED;
    msg.timestampEnd = xTaskGetTickCount(); /* ok for demo */

    /* IMPORTANT: snprintf must use "%s" format */
    snprintf(msg.handledBy, sizeof(msg.handledBy), "%s", pcTaskGetName(NULL));

    (void)xQueueSend(handle_clientUDPTxQ, &msg, 0);

    printf("[Client][%s] CANCELLED event id=%u prio=%u (overload)\n",
           pcTaskGetName(NULL), (unsigned)cancelled.eventID, (unsigned)cancelled.priority);

    return pdTRUE;
}

/* Reorder queue by priority (high first), stable within same priority.
   This is the simplest “reassign” without per-vehicle inbox queues.
*/
static void ReorderQueueByPriority(const DepartmentDescription_t *d)
{
    EmergencyEvent_t buf[DEPT_Q_LEN];
    UBaseType_t n = 0;

    xSemaphoreTake(d->mutex, portMAX_DELAY);

    n = uxQueueMessagesWaiting(d->queue);
    if (n == 0) {
        xSemaphoreGive(d->mutex);
        return;
    }
    if (n > DEPT_Q_LEN) n = DEPT_Q_LEN;

    for (UBaseType_t i = 0; i < n; i++) {
        if (xQueueReceive(d->queue, &buf[i], 0) != pdPASS) {
            n = i;
            break;
        }
    }

    /* Stable insertion sort: higher priority first */
    for (UBaseType_t i = 1; i < n; i++) {
        EmergencyEvent_t key = buf[i];
        int j = (int)i - 1;
        while (j >= 0 && buf[j].priority < key.priority) {
            buf[j + 1] = buf[j];
            j--;
        }
        buf[j + 1] = key;
    }

    for (UBaseType_t i = 0; i < n; i++) {
        (void)xQueueSend(d->queue, &buf[i], 0);
    }

    xSemaphoreGive(d->mutex);

    printf("[Client][%s] Reordered queue by priority (n=%u)\n",
           pcTaskGetName(NULL), (unsigned)n);
}

// TODO: Task_Manager_Departments_X
//   1. Fix task manger interrupt
//   2. Fix break and return policy
//   3. Maybe add extra feature

void Task_Manager_Departments_X(void *pvParameters)
{
    DepartmentDescription_t *d = (DepartmentDescription_t *)pvParameters;
    if (d == NULL || d->queue == NULL || d->mutex == NULL || d->availableSem == NULL) {
        printf("[Client][MANAGER] Bad params -> deleting task\n");
        vTaskDelete(NULL);
    }

    const UBaseType_t maxVehicles = DeptMaxVehicles(d->type);

    /* How many tokens we “stole” for break (vehicles unavailable by policy) */
    UBaseType_t breakCount = 0;

    TickType_t lastBusyTick = xTaskGetTickCount();

    printf("[Client][%s] Started (dept=%s maxVehicles=%u)\n",
           pcTaskGetName(NULL), d->name, (unsigned)maxVehicles);

    for (;;) {
        const UBaseType_t qLen = uxQueueMessagesWaiting(d->queue);

        if (qLen > 0) {
            lastBusyTick = xTaskGetTickCount();
        }

        /* ---------------- Overload policy ---------------- */
        if (qLen >= OVERLOAD_THRESHOLD) {

            /* Ensure no one is on break during overload */
            while (breakCount > 0) {
                xSemaphoreGive(d->availableSem);
                breakCount--;
                printf("[Client][%s] Dept=%s -> RETURN from break (breakCount=%u)\n",
                       pcTaskGetName(NULL), d->name, (unsigned)breakCount);
            }

            /* Reorder so vehicles will take high priority first */
            ReorderQueueByPriority(d);

            /* Cancel lowest-priority events until we reduce backlog */
            UBaseType_t cancelDone = 0;
            while (uxQueueMessagesWaiting(d->queue) > OVERLOAD_TARGET &&
                   cancelDone < MAX_CANCEL_PER_CYCLE) {

                if (CancelOneLowestPriorityEvent(d) == pdTRUE) {
                    cancelDone++;
                } else {
                    break;
                }
            }
        }

        /* ---------------- Break / return policy (simple) ----------------
           If system is idle (queue empty for some time), allow some vehicles “on break”.
           If queue has work and we have break tokens -> return them immediately.
        */
        static TickType_t lastNonEmptyTick = 0;

        if (lastNonEmptyTick == 0) {
            lastNonEmptyTick = xTaskGetTickCount();
        }

        TickType_t now = xTaskGetTickCount();

        if (qLen > 0) {
            lastNonEmptyTick = now;
        
            /* If backlog exists, immediately return all vehicles from break */
            while (breakCount > 0) {
                xSemaphoreGive(d->availableSem);
                breakCount--;
                printf("[Client][%s] Dept=%s -> RETURN from break (breakCount=%u)\n",
                       pcTaskGetName(NULL), d->name, (unsigned)breakCount);
            }
        }
        else {
            /* Queue empty */
            const BaseType_t idleLongEnough =
                (now - lastNonEmptyTick) >= pdMS_TO_TICKS(BREAK_IDLE_MS);
        
            const UBaseType_t maxBreak = (maxVehicles * BREAK_MAX_FRACTION_NUM) / BREAK_MAX_FRACTION_DEN;
        
            if (idleLongEnough && breakCount < maxBreak) {
                if (xSemaphoreTake(d->availableSem, 0) == pdPASS) {
                    breakCount++;
                    printf("[Client][%s] Dept=%s -> vehicle ON BREAK (breakCount=%u)\n",
                           pcTaskGetName(NULL), d->name, (unsigned)breakCount);
                }
            }
        }
    }
    
        vTaskDelete(NULL); // Should never reach here

}