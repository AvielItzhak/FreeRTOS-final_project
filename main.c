/**
 * @file main.c
 * @brief Project entry point: initializes system and starts only the Server task.
 */

#include <stdio.h>

#include "init.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Server/Server_Task.h"

int main(void)
{
    printf("MAIN: start\n");

    init_main();
    printf("MAIN: after init_main\n");

    /* Create Server task only */
    BaseType_t rc = xTaskCreate(
        vServerTask,
        "Server_Gen",
        2048,
        NULL,
        2,
        NULL
    );

    printf("MAIN: xTaskCreate(Server) = %ld\n", (long)rc);

    printf("MAIN: starting scheduler\n");
    vTaskStartScheduler();

    /* Should never reach here */
    printf("MAIN: scheduler returned (unexpected)\n");
    return 0;
}
