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
