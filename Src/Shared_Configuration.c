/**
 * @file Shared_Configuration.c
 * @brief Shared configuration definitions between server and client.
 *        This file contains the definition of queue handles and Queue creation function implementation.
 * 
 * @attention This file is used by both server and client modules.
 */

#include "Shared_Configuration.h"


/* ------Queue implementation------ */

/* Define queue handles and set to NULL */
QueueHandle_t handle_serverUDPTxQ = NULL;
QueueHandle_t handle_serverUDPRxQ = NULL;
QueueHandle_t handle_clientUDPRxQ = NULL;
QueueHandle_t handle_clientUDPTxQ = NULL;

/* Function to create all queues */
BaseType_t CreateQueues(void) 
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
} /* End of CreateQueues */


