/**
 * @file Shared_Configuration.h
 * @brief Shared configuration definitions between server and client.
 *        This file contains common data structures and constants used for
 *        communication between the server and client applications.
 *        Also, it defines the queue handles for inter-task communication.
 * 
 * @attention This file is used by both server and client modules.
 */

#ifndef SHARED_CONFIGURATION_H
#define SHARED_CONFIGURATION_H

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS Core Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Event types */
typedef enum {
    EVENT_AMBULANCE = 0,
    EVENT_POLICE = 1,
    EVENT_FIRE_DEPARTMENT = 2,
    EVENT_MAINTENANCE = 3,
    EVENT_WASTE_COLLECTION = 4,
    EVENT_ELECTRICITY = 5,
    EVENT_MAX
} EventType_t;

/* Structure for emergency event from server to client */
typedef struct {
    uint32_t eventID;
    EventType_t type;
    uint8_t priority;
    char location[32];
    uint32_t timestampStart;
} EmergencyEvent_t;

/* Structure for completion message from client to server */
typedef struct {
    uint32_t eventID;
    char handledBy[16];
    uint32_t timestampEnd;
    uint8_t status; // 1 = Success, 0 = Failed
} CompletionMsg_t;


/* Queues (Server <-> Client) lengths */
#define SERVER_UDP_TX_LEN   16
#define SERVER_UDP_RX_LEN   16
#define CLIENT_UDP_RX_LEN   16
#define CLIENT_UDP_TX_LEN   16

/* Queue handles - extern in order to use in both server and client */
extern QueueHandle_t handle_serverUdpTxQ;  /* use for EmergencyEvent_t */
extern QueueHandle_t handle_serverUdpRxQ;  /* use for CompletionMsg_t  */
extern QueueHandle_t handle_clientUdpRxQ;  /* use for EmergencyEvent_t */
extern QueueHandle_t handle_clientUdpTxQ;  /* use for CompletionMsg_t  */

/* Create all queues */
BaseType_t CreateQueues(void);


#endif // SHARED_CONFIGURATION_H