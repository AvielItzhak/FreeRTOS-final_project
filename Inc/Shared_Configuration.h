/**
 * @file Shared_Configuration.h
 * @brief Shared configuration definitions between server and client.
 *        This file contains common data structures and constants used for
 *        communication between the server and client applications.
 *        Also, it defines the queue handles for inter-task communication (UDP & Dispatcher to Departments).
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

/* -------Global Delay Definitions------ */

#define Short_Delay_MS     10   // 10 ms
#define Medium_Delay_MS    100  // 100 ms
#define Long_Delay_MS      500  // 500 ms

/* -------UDP Setup------- */

#define UDP_IP_Addr          "127.0.0.1" // Loopback IP 
#define UDP_SERVER_PORT      5000   // Server PORT 
#define UDP_CLIENT_PORT      5001   // Client PORT


/* --------Data Structures------- */

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


/* --------Queues (Server <-> Client)------- */

#define SERVER_UDP_TX_LEN   16  // Length of Server UDP TX Queue
#define SERVER_UDP_RX_LEN   16  // Length of Server UDP RX Queue
#define CLIENT_UDP_RX_LEN   16  // Length of Client UDP RX Queue
#define CLIENT_UDP_TX_LEN   16  // Length of Client UDP TX Queue

/* Queue handles - extern in order to use in both server and client */
extern QueueHandle_t handle_serverUDPTxQ;  /* use for EmergencyEvent_t */
extern QueueHandle_t handle_serverUDPRxQ;  /* use for CompletionMsg_t  */
extern QueueHandle_t handle_clientUDPRxQ;  /* use for EmergencyEvent_t */
extern QueueHandle_t handle_clientUDPTxQ;  /* use for CompletionMsg_t  */


/* --------Queues (Dispatcher <-> Departments)-------- */

#define DEPT_Q_LEN  16 // Length of each Department Queue

extern QueueHandle_t handle_deptAmbulanceQ;  /* use for EmergencyEvent_t */
extern QueueHandle_t handle_deptPoliceQ;     /* use for EmergencyEvent_t */
extern QueueHandle_t handle_deptFireQ;       /* use for EmergencyEvent_t */
extern QueueHandle_t handle_deptMaintQ;      /* use for EmergencyEvent_t */
extern QueueHandle_t handle_deptElectQ;      /* use for EmergencyEvent_t */
extern QueueHandle_t handle_deptWasteQ;      /* use for EmergencyEvent_t */

extern SemaphoreHandle_t handleMux_deptAmbulanceQ;  // Mutex for Ambulance queue
extern SemaphoreHandle_t handleMux_deptPoliceQ;     // Mutex for Police queue
extern SemaphoreHandle_t handleMux_deptFireQ;       // Mutex for Fire Department queue
extern SemaphoreHandle_t handleMux_deptMaintQ;      // Mutex for Maintenance queue
extern SemaphoreHandle_t handleMux_deptElectQ;      // Mutex for Electricity queue
extern SemaphoreHandle_t handleMux_deptWasteQ;      // Mutex for Waste Collection queue

/* Create UDP & Client department queues */
BaseType_t CreateClientDepartmentQueuesAndMutex(void);
BaseType_t CreateUDPQueues(void);


/* ------UDP Tasks definitions------ */

void vServerUDPTxTask(void *pvParameters); // Server UDP TX Task
void vServerUDPRxTask(void *pvParameters); // Server UDP RX Task
void vClientUDPRxTask(void *pvParameters); // Client UDP RX Task
void vClientUDPTxTask(void *pvParameters); // Client UDP TX Task

#endif // SHARED_CONFIGURATION_H