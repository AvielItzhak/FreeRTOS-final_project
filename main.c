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


    /* Create Server <--> Client MSG Queues */

    BaseType_t Queues_Check = CreateQueues();
    if (Queues_Check != pdPASS) {
        printf("[MAIN] Failed to create queues\n");
        return -30;
    }

    
    /* --------Tasks Creation---------- */

    /* Create UDP tasks */

    if ((xTaskCreate( vServerUDPTxTask, "Server_UDP_Tx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ServerUDP_Tx_Task) Failed!\n");
        return -21;
    }
    else { printf("[MAIN] xTaskCreate(ServerUDP_Tx_Task) Successful\n"); }

    if ((xTaskCreate( vServerUDPRxTask, "Server_UDP_Rx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ServerUDP_Rx_Task) Failed!\n");
        return -22;
    }
    else { printf("[MAIN] xTaskCreate(ServerUDP_Rx_Task) Successful\n"); }

    if ((xTaskCreate( vClientUDPTxTask, "Client_UDP_Tx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ClientUDP_Tx_Task) Failed!\n");
        return -24;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_TxTask) Successful\n"); }

    if ((xTaskCreate( vClientUDPRxTask, "Client_UDP_Rx", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ClientUDP_Rx_Task) Failed!\n");
        return -25;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_RxTask) Successful\n"); }

    if ((xTaskCreate( vClientEchoTask, "Client_UDP_Echo", configMINIMAL_STACK_SIZE,
                     NULL, MEDIUM_PRIORITY, NULL) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ClientUDP_Echo_Task) Failed!\n");
        return -26;
    }
    else { printf("[MAIN] xTaskCreate(ClientUDP_Echo_Task) Successful\n"); }
    /* Create Server Event Generator Task */

    if ((xTaskCreate( Task_EventGenerator, "Server_Event_Gen",configMINIMAL_STACK_SIZE, 
                     NULL, HIGH_PRIORITY, &xServerEventGenTaskHandle) != pdPASS))
    { // Failed to create task
        printf("[MAIN] xTaskCreate(ServerTask) Failed!\n");
        return -23;
    }
    else { printf("[MAIN] xTaskCreate(ServerTask) Successful\n"); } 



    /* Start Scheduler */
    printf("[MAIN] starting scheduler\n--------------------------------\n\n");
    vTaskStartScheduler();


    /* Should never reach here! - Print & Endless Loop */
    printf("\n\n[MAIN] Scheduler returned - Unexpected Error\n");
    while (1) {} // Endless loop
    
    return 100; // Return 100 on unexpected exit
}
