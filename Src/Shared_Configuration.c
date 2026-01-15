/**
 * @file Shared_Configuration.c
 * @brief Shared configuration definitions between server and client.
 * 
 * @attention This file is used by both server and client modules.
 */

#include "Shared_Configuration.h"

QueueHandle_t handle_serverUdpTxQ = NULL;
QueueHandle_t handle_serverUdpRxQ = NULL;
QueueHandle_t handle_clientUdpRxQ = NULL;
QueueHandle_t handle_clientUdpTxQ = NULL;

BaseType_t CreateQueues(void)
{
    handle_serverUdpTxQ = xQueueCreate(SERVER_UDP_TX_LEN, sizeof(EmergencyEvent_t));
    handle_serverUdpRxQ = xQueueCreate(SERVER_UDP_RX_LEN, sizeof(CompletionMsg_t));

    handle_clientUdpRxQ = xQueueCreate(CLIENT_UDP_RX_LEN, sizeof(EmergencyEvent_t));
    handle_clientUdpTxQ = xQueueCreate(CLIENT_UDP_TX_LEN, sizeof(CompletionMsg_t));

    /* Check if all queues were created successfully */
    if (!handle_serverUdpTxQ || !handle_serverUdpRxQ || !handle_clientUdpRxQ || !handle_clientUdpTxQ) {
        printf("[Shared] ERROR: Failed to create one or more queues\n");
        return pdFAIL;
    }

    printf("[Shared] MSG queues created successfully\n");
    return pdPASS;
}
