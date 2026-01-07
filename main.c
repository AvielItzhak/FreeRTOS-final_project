/**
 * @file main.c
 * @brief Project entry point: initializes system and starts only the Server task.
 */

#include <stdio.h>

#include "init.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Server/Server_Task.h"

// Priorities:
#define HIGH_PRIORITY      4
#define MEDIUM_PRIORITY    3
#define NORMAL_PRIORITY    2
#define LOW_PRIORITY       1

// Task Handles:
TaskHandle_t xServerEventGenTaskHandle = NULL;


int main(void)
{
    printf("MAIN: start\n");

    init_main();
    printf("MAIN: after init_main\n");

    /* ---------- Tasks Creation --------- */

    // Create Server Task
    if ((xTaskCreate( vServerEventGen, "Server_Event_Gen",configMINIMAL_STACK_SIZE, 
                     NULL, NORMAL_PRIORITY, &xServerEventGenTaskHandle) != pdPASS))
    { // Error
        printf("MAIN: xTaskCreate(ServerTask) Failed!\n");
        return -1;
    }
    else { printf("MAIN: xTaskCreate(ServerTask) Successful\n"); } // Success print

    
    

    // Start the scheduler
    printf("MAIN: starting scheduler\n--------------------------------\n\n");
    vTaskStartScheduler();


    /* Should never reach here! - Print & Endless Loop */
    printf("\n\nMAIN: scheduler returned (unexpected)\n");
    while (1) {}
    
    return 0;
}
