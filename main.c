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
#include "Client/Dispatcher_Task.h"
#include "Client/Vehicle_Task.h"

// Priorities: 
#define HIGH_PRIORITY      4
#define MEDIUM_PRIORITY    3
#define NORMAL_PRIORITY    2
#define LOW_PRIORITY       1

// Task Handles:
TaskHandle_t xServerEventGenTaskHandle = NULL; // Server Event Generator Task Handle


int main(void)
{
    printf("[MAIN] Start Main program\n");
    
    init_main(); // System Initialization
    ServerUDP_Init(); // Initialize Server UDP
    ClientUDP_Init(); // Initialize Client UDP

    printf("[MAIN] System initialized\n");


    /* Create all message queues and mutexes */

    BaseType_t Queues_Check = CreateUDPQueues(); // Server <--> Client queues
    if (Queues_Check != pdPASS) {
        printf("[MAIN] Failed to create queues\n");
        return -31;
    }

    BaseType_t Dept_QueuesAndMutex_Check = CreateClientDepartmentQueuesAndMutex(); // Client Department queues and mutexes
    if (Dept_QueuesAndMutex_Check != pdPASS) {
    printf("[MAIN] Failed to create client department queues\n");
    return -32;
    }

    
    /* --------Tasks Creation---------- */

    /* Create UDP tasks */

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

    /*if ((xTaskCreate( vClientEchoTask, "Client_UDP_Echo", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientUDP_Echo_Task) Failed!\n");
        return -25;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_Echo_Task) Successful\n"); } // Successful creation of Client UDP Echo Task

    /* Create Server Event Generator Task */
    if ((xTaskCreate( Task_EventGenerator, "Server_Event_Gen",configMINIMAL_STACK_SIZE, 
                     NULL, HIGH_PRIORITY, &xServerEventGenTaskHandle) != pdPASS))
    {
        printf("[MAIN] xTaskCreate(ServerTask) Failed!\n");
        return -26;
    }
    else { printf("[MAIN] xTaskCreate(ServerTask) Successful\n"); } // Successful creation of Server Event Generator Task

    /* Create Client Dispatcher Task */
    if ((xTaskCreate( Task_Dispatcher, "Client_Dispatcher", configMINIMAL_STACK_SIZE,
                     NULL, NORMAL_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientDispatcher_Task) Failed!\n");
        return -27;
    }
    else { printf("[MAIN] xTaskCreate(ClientDispatcher_Task) Successful\n"); } // Successful creation of Client Dispatcher Task

    /* Create Client Vehicle Task */
    if ((xTaskCreate( Task_TestVehicle, "Client_Vehicle", configMINIMAL_STACK_SIZE,
                     NULL, NORMAL_PRIORITY, NULL) != pdPASS))
    { 
        printf("[MAIN] xTaskCreate(ClientVehicle_Task) Failed!\n");
        return -28;
    }
    else { printf("[MAIN] xTaskCreate(ClientVehicle_Task) Successful\n"); } // Successful creation of Client Vehicle Task

    
    /* Start Scheduler */
    printf("\n[MAIN] starting scheduler\n--------------------------------\n");
    vTaskStartScheduler();


    /* Should never reach here! - Print & Endless Loop */
    printf("\n\n[MAIN] Scheduler returned - Unexpected Error\n");
    while (1) {} // Endless loop
    
    return 100; // Return 100 on unexpected exit
}
