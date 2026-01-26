/**
 * @file main.c
 * @authors Aviel Yitzhak (Aviel2488@gmail.com)
 * @brief Project entry point: initializes system and starts only the Server task.
 */

#include <stdio.h>

#include "init.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Server/Server_Task.h"
#include "Server/Server_UDP.h"
#include "Client/Client_UDP.h"
#include "Client/DispatcherAndMangerDepartment_Task.h"
#include "Client/Vehicle_Task.h"

// Priorities: 
#define HIGH_PRIORITY      4
#define MEDIUM_PRIORITY    3
#define NORMAL_PRIORITY    2
#define LOW_PRIORITY       1

// Task Handles:
TaskHandle_t xServerEventGenTaskHandle            = NULL; // Server Event Generator Task Handle
TaskHandle_t xClientDispatcherTaskHandle          = NULL; // Client Dispatcher Task Handle
TaskHandle_t xClientManagerTaskHandle[6]          = {NULL}; // Client Manager Task Handle array for 6 departments

TaskHandle_t xClientAmbulanceTaskHandle[AMBULANCE_VEHICLES]            = {NULL}; // Client Ambulance Task Handle array
TaskHandle_t xClientPoliceTaskHandle[POLICE_VEHICLES]                  = {NULL}; // Client Police Task Handle array
TaskHandle_t xClientFireTaskHandle[FIRE_VEHICLES]                      = {NULL}; // Client Fire Task Handle array
TaskHandle_t xClientMaintenanceTaskHandle[MAINTENANCE_VEHICLES]        = {NULL}; // Client Maintenance Task Handle array
TaskHandle_t xClientWasteTaskHandle[WASTE_VEHICLES]                    = {NULL}; // Client Waste Collection Task Handle array
TaskHandle_t xClientElectricityTaskHandle[ELECTRICITY_VEHICLES]        = {NULL}; // Client Electricity Task Handle array

/* Department Description Array - Helper array */
DepartmentDescription_t deptDesc[] = {
    { "AMBULANCE",   EVENT_AMBULANCE,        NULL, NULL, NULL },
    { "POLICE",      EVENT_POLICE,           NULL, NULL, NULL },
    { "FIRE",        EVENT_FIRE_DEPARTMENT,  NULL, NULL, NULL },
    { "MAINT",       EVENT_MAINTENANCE,      NULL, NULL, NULL },
    { "WASTE",       EVENT_WASTE_COLLECTION, NULL, NULL, NULL },
    { "ELECTRICITY", EVENT_ELECTRICITY,      NULL, NULL, NULL },
};

const size_t numDepts = sizeof(deptDesc) / sizeof(deptDesc[0]);

int main(void)
{
    printf("[MAIN] Start Main program\n");

    /* Initialize all components */
    init_main(); // System Initialization
    ServerUDP_Init(); // Initialize Server UDP
    ClientUDP_Init(); // Initialize Client UDP



    /* Create all message queues, mutexes and counting semaphores */

    BaseType_t Queues_Check = CreateUDPQueues(); // Server <--> Client queues
    if (Queues_Check != pdPASS) {
        printf("[MAIN] Failed to create queues\n");
        return -31;
    }

    BaseType_t Dept_QueuesSemaphoresAndMutex_Check = CreateClientDepartmentQueuesSemaphoresAndMutex(); // Client Department queues and mutexes
    if (Dept_QueuesSemaphoresAndMutex_Check != pdPASS) {
    printf("[MAIN] Failed to create client department queues\n");
    return -32;
    }

    printf("[MAIN] Queues, Semaphores and Mutexes created successfully\n");

    ClientDeptManager_Init(deptDesc); // Initialize Client Department Manager

    printf("[MAIN] System initialized\n");

    
    /* --------Tasks Creation---------- */

    /* Create UDP tasks */
    printf("[MAIN] Starting Tasks creations ...\n");

    if ((xTaskCreate( vServerUDPTxTask, "Server_UDP_Tx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ServerUDP_Tx_Task) Failed!\n");
        return -21;
    }
    else { printf("[MAIN] xTaskCreate(ServerUDP_Tx_Task) Successful\n"); } // Successful creation of Server UDP TX Task

    if ((xTaskCreate( vServerUDPRxTask, "Server_UDP_Rx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ServerUDP_Rx_Task) Failed!\n");
        return -22;
    }
    else { printf("[MAIN] xTaskCreate(ServerUDP_Rx_Task) Successful\n"); } // Successful creation of Server UDP RX Task

    if ((xTaskCreate( vClientUDPTxTask, "Client_UDP_Tx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientUDP_Tx_Task) Failed!\n");
        return -23;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_TxTask) Successful\n"); } // Successful creation of Client UDP TX Task

    if ((xTaskCreate( vClientUDPRxTask, "Client_UDP_Rx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientUDP_Rx_Task) Failed!\n");
        return -24;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_RxTask) Successful\n"); } // Successful creation of Client UDP RX Task

   
    /* Create Server Event Generator Task */
    if ((xTaskCreate( Task_EventGenerator, "Server_Event_Gen",configMINIMAL_STACK_SIZE, 
                     NULL, HIGH_PRIORITY, &xServerEventGenTaskHandle) != pdPASS))
    {
        printf("[MAIN] xTaskCreate(ServerTask) Failed!\n");
        return -26;
    }
    else { printf("[MAIN] xTaskCreate(ServerTask) Successful\n"); } // Successful creation of Server Event Generator Task


    /* Create Client Dispatcher & Manager Task */
    if ((xTaskCreate( Task_Dispatcher, "Client_Dispatcher", configMINIMAL_STACK_SIZE,
                     NULL, NORMAL_PRIORITY, &xClientDispatcherTaskHandle) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientDispatcher_Task) Failed!\n");
        return -27;
    }
    else { printf("[MAIN] xTaskCreate(ClientDispatcher_Task) Successful\n"); } // Successful creation of Client Dispatcher Task

    for (size_t i = 0; i < 6; i++)
    {   
        // Create unique task name for each department manager task
        char taskName[32] = {0};
        sprintf(taskName, "MANAGER_%s", deptDesc[i].name); // depNames from Shared_Configuration.h

        if ((xTaskCreate( Task_Manager_Departments_X, taskName, configMINIMAL_STACK_SIZE,
                     &deptDesc[i], LOW_PRIORITY, &xClientManagerTaskHandle[i]) != pdPASS))
            { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -41;
            }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Ambulance Task
    }



    /* Create Client Department Tasks - Based on numbers of vehicles in each department */
    for (size_t i = 0; i < AMBULANCE_VEHICLES; i++)
    {   
        // Create unique task name for each Ambulance vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "AMBULANCE_%zu", i+1);

        if ((xTaskCreate( Task_Ambulance_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientAmbulanceTaskHandle[i]) != pdPASS))
            { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -41;
            }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Ambulance Task
    }

    for (size_t i = 0; i < POLICE_VEHICLES; i++)
    {
        // Create unique task name for each Police vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "POLICE_%zu", i+1);

        if ((xTaskCreate( Task_Police_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientPoliceTaskHandle[i]) != pdPASS))
        { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -42;
        }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Police Task
    }

    for (int i=0; i<FIRE_VEHICLES; i++)
    {
        // Create unique task name for each Fire vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "FIRE_%d", i+1);

        if ((xTaskCreate( Task_Fire_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientFireTaskHandle[i]) != pdPASS))
        { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -43;
        }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Fire Task
    }

    for (size_t i = 0; i < MAINTENANCE_VEHICLES; i++)
    {
        // Create unique task name for each Maintenance vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "MAINTENANCE_%zu", i+1);

        if ((xTaskCreate( Task_Maintenance_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientMaintenanceTaskHandle[i]) != pdPASS))
        { 
           printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
           return -44;
        }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Maintenance Task
    }
    
    for (size_t i = 0; i < WASTE_VEHICLES; i++)
    {
        // Create unique task name for each Waste Collection vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "WASTE_%zu", i+1);
        
        if ((xTaskCreate( Task_Waste_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientWasteTaskHandle[i]) != pdPASS))
        { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -45;
        }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Waste Task
    }

    for (size_t i = 0; i < ELECTRICITY_VEHICLES; i++)
    {
        // Create unique task name for each Electricity vehicle task
        char taskName[24] = {0};
        sprintf(taskName, "ELECTRICITY_%zu", i+1);

        if ((xTaskCreate( Task_Electricity_X, taskName, configMINIMAL_STACK_SIZE,
                     NULL, LOW_PRIORITY, &xClientElectricityTaskHandle[i]) != pdPASS))
        { 
            printf("[MAIN] xTaskCreate(Client_%s) Failed!\n", taskName);
            return -46;
        }
        else { printf("[MAIN] xTaskCreate(Client_%s) Successful\n", taskName); } // Successful creation of Client Electricity Task
    }
    


    /* Start Scheduler */
    printf("\n[MAIN] starting scheduler\n--------------------------------\n");
    vTaskStartScheduler();


    /* Should never reach here! - Print & Endless Loop */
    printf("\n\n[MAIN] Scheduler returned - Unexpected Error\n");
    while (1) {} // Endless loop
    
    return 100; // Return 100 on unexpected exit
}
