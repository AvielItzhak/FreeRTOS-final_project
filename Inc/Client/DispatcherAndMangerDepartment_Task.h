/**
 * @file DispatcherAndMangerDepartment_Task.h
 * @brief Declares the Dispatcher and Manager Department task functions.
 * 
 * @attention
 */

#ifndef DISPATCHER_AND_MANAGER_DEPARTMENT_TASK_H
#define DISPATCHER_AND_MANAGER_DEPARTMENT_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "Shared_Configuration.h"   // EventType_t, queues/sems handles, etc.
#include "event_groups.h"          // Event groups

/* --- Simple policy knobs --- */
#define MANAGER_POLL_MS              50
#define OVERLOAD_THRESHOLD           12   /* when queue len >= this -> overload mode */
#define OVERLOAD_TARGET              8    /* after cancelling, try to get down to this */
#define MAX_CANCEL_PER_CYCLE         2    /* avoid spending too long in one loop */
#define BREAK_IDLE_MS                3000 /* if idle long enough -> allow breaks */
#define BREAK_MAX_FRACTION_NUM       1    /* allow up to 1/2 vehicles on break */
#define BREAK_MAX_FRACTION_DEN       2

/* If you want to notify server on cancel, use status=1 for "not success".
   (Your code/comments are inconsistent on 0/1 meaning; server prints status=0 currently.)
*/
#define STATUS_CANCELLED             1

/* EventGroup bits */
#define MGR_BIT_OVERLOAD            (1u << 0) // overload mode active


/* If you have a global dept array, declare it extern here (recommended) */
extern DepartmentDescription_t g_depts[EVENT_MAX];

/**
 * @brief Receives events from UDP RX queue and forwards them to the appropriate department queue based on event type.
 * @attention This task priority is set to be higher than department tasks - Medium Level.
 * @param pvParameters Not used pointer.
 */
void Task_Dispatcher(void *pvParameters);

/**
 * @brief Monitors department queues and vehicle availability, and manages dispatching logic.
 * @attention This task priority is set to be higher than department tasks - Medium Level.
 * @param pvParameters Pointer to DepartmentDescription_t array.
 */
void Task_Manager_Departments_X(void *pvParameters);

/**
 * @brief Initializes department description table with queues, mutexes, and semaphores.
 * @param pvParameters Pointer to DepartmentDescription_t array.
 */
void ClientDeptManager_Init(DepartmentDescription_t *pvParameters);



#endif // DISPATCHER_AND_MANAGER_DEPARTMENT_TASK_H
