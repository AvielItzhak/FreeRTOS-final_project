/**
 * @file Shared_Configuration.c
 * @brief Shared configuration definitions between server and client.
 *        This file contains the definition of queue handles and Queue creation function implementation.
 * 
 * @attention This file is used by both server and client modules.
 */

#include "Shared_Configuration.h"


/* ------Queue implementation------ */

/* Define queue, semaphore and mutex handles and set to NULL */
QueueHandle_t handle_serverUDPTxQ      = NULL;
QueueHandle_t handle_serverUDPRxQ      = NULL;
QueueHandle_t handle_clientUDPRxQ      = NULL;
QueueHandle_t handle_clientUDPTxQ      = NULL;

QueueHandle_t handle_deptAmbulanceQ    = NULL;
QueueHandle_t handle_deptPoliceQ       = NULL;
QueueHandle_t handle_deptFireQ         = NULL;
QueueHandle_t handle_deptMaintQ        = NULL;
QueueHandle_t handle_deptElectQ        = NULL;
QueueHandle_t handle_deptWasteQ        = NULL;

SemaphoreHandle_t handleMux_deptAmbulanceQ = NULL;
SemaphoreHandle_t handleMux_deptPoliceQ    = NULL;
SemaphoreHandle_t handleMux_deptFireQ      = NULL;
SemaphoreHandle_t handleMux_deptMaintQ     = NULL;
SemaphoreHandle_t handleMux_deptElectQ     = NULL;
SemaphoreHandle_t handleMux_deptWasteQ     = NULL;

SemaphoreHandle_t handleSem_deptAmbulance = NULL;
SemaphoreHandle_t handleSem_deptPolice    = NULL;
SemaphoreHandle_t handleSem_deptFire      = NULL;
SemaphoreHandle_t handleSem_deptMaint     = NULL;
SemaphoreHandle_t handleSem_deptElect     = NULL;
SemaphoreHandle_t handleSem_deptWaste     = NULL;



/* Function to create all UDP queues */
BaseType_t CreateUDPQueues(void) 
{
    handle_serverUDPTxQ = xQueueCreate(SERVER_UDP_TX_LEN, sizeof(EmergencyEvent_t));
    handle_serverUDPRxQ = xQueueCreate(SERVER_UDP_RX_LEN, sizeof(CompletionMsg_t));

    handle_clientUDPRxQ = xQueueCreate(CLIENT_UDP_RX_LEN, sizeof(EmergencyEvent_t));
    handle_clientUDPTxQ = xQueueCreate(CLIENT_UDP_TX_LEN, sizeof(CompletionMsg_t));

    /* Check if all queues were created successfully */
    if (!handle_serverUDPTxQ || !handle_serverUDPRxQ || !handle_clientUDPRxQ || !handle_clientUDPTxQ) {
        printf("[Shared] ERROR: Failed to create one or more queues\n");
        return pdFAIL;
    }

    printf("[Shared] MSG queues created successfully\n");
    return pdPASS;
} /* End of CreateUDPQueues */

/* Function to create all Department queues, mutexes and counting semaphores */
BaseType_t CreateClientDepartmentQueuesSemaphoresAndMutex(void) 
{
    handle_deptAmbulanceQ = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t));
    handle_deptPoliceQ    = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t));
    handle_deptFireQ      = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t));
    handle_deptMaintQ     = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t));
    handle_deptElectQ     = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t));
    handle_deptWasteQ     = xQueueCreate(DEPT_Q_LEN, sizeof(EmergencyEvent_t)); 

    handleMux_deptAmbulanceQ = xSemaphoreCreateMutex();
    handleMux_deptPoliceQ    = xSemaphoreCreateMutex();
    handleMux_deptFireQ      = xSemaphoreCreateMutex();
    handleMux_deptMaintQ     = xSemaphoreCreateMutex();
    handleMux_deptElectQ     = xSemaphoreCreateMutex();
    handleMux_deptWasteQ     = xSemaphoreCreateMutex();

    handleSem_deptAmbulance = xSemaphoreCreateCounting(AMBULANCE_VEHICLES, AMBULANCE_VEHICLES);
    handleSem_deptPolice    = xSemaphoreCreateCounting(POLICE_VEHICLES, POLICE_VEHICLES);
    handleSem_deptFire      = xSemaphoreCreateCounting(FIRE_VEHICLES, FIRE_VEHICLES);
    handleSem_deptMaint     = xSemaphoreCreateCounting(MAINTENANCE_VEHICLES, MAINTENANCE_VEHICLES);
    handleSem_deptElect     = xSemaphoreCreateCounting(ELECTRICITY_VEHICLES, ELECTRICITY_VEHICLES);
    handleSem_deptWaste     = xSemaphoreCreateCounting(WASTE_VEHICLES, WASTE_VEHICLES); 

    /* Check if all queues, mutexes and semaphores were created successfully */
    if (!handle_deptAmbulanceQ || !handle_deptPoliceQ || !handle_deptFireQ || !handle_deptMaintQ || !handle_deptElectQ || !handle_deptWasteQ ||
        !handleMux_deptAmbulanceQ || !handleMux_deptPoliceQ || !handleMux_deptFireQ || !handleMux_deptMaintQ || !handleMux_deptElectQ || 
        !handleMux_deptWasteQ || !handleSem_deptAmbulance || !handleSem_deptPolice || !handleSem_deptFire || !handleSem_deptMaint || 
        !handleSem_deptElect || !handleSem_deptWaste) 
    {
        printf("[Shared] ERROR: Failed to create one or more department queues, semaphores or mutexes\n");
        return pdFAIL; // return failure if any creation failed
    }

    printf("[Shared] Department queues, semaphores and mutexes created successfully\n");
    return pdPASS; // return success if all creations succeeded

} /* End of CreateClientDepartmentQueuesSemaphoresAndMutex */


