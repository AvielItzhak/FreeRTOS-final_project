/**
 * @file Vehicle_Task.h
 * @brief This header file contains the different Department's Vehicle tasks.
 *        Thus task to simulates Department's Vehicles <-> Dispatcher communication. 
 *        Typical Task flows these logic:
 *             1) Wait for an event to exist (do NOT remove it yet) - use xQueuePeek to check the queue head.
 *             2) Get the resource semaphore for this department/type - use DeptSemFromType() to get the correct counting semaphore handle for this event type.
 *             3) Wait until a vehicle/resource is available (manager "break" will block us here) - use xSemaphoreTake on the counting semaphore to wait for availability.
 *             4) Remove the event from the queue (now that we know we can handle it) - use xQueueReceive to actually pull the event from the queue.
 * 
 * @attention Task_TestVehicle was only used for testing.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 */

#ifndef VEHICLE_TASK_H
#define VEHICLE_TASK_H

/**
 * @brief This task is for testing purposes only.
 * @param pvParameters 
 */
void Task_TestVehicle(void *pvParameters);


/**
 * @brief This task to simulates Ambulance <-> Dispatcher communication.
 * @param pvParameters 
 */
void Task_Ambulance_X(void *pvParameters);

/**
 * @brief This task to simulates Police <-> Dispatcher communication.
 * @param pvParameters Not used pointer.
 */
void Task_Police_X(void *pvParameters);

/**
 * @brief This task to simulates Fire Department <-> Dispatcher communication.
 * @param pvParameters 
 */
void Task_Fire_X(void *pvParameters);

/**
 * @brief This task to simulates Maintenance <-> Dispatcher communication.
 * @param pvParameters 
 */
void Task_Maintenance_X(void *pvParameters);

/**
 * @brief This task to simulates Waste Collection <-> Dispatcher communication.
 * @param pvParameters 
 */
void Task_Waste_X(void *pvParameters);

/**
 * @brief This task to simulates Electricity Department <-> Dispatcher communication.
 * @param pvParameters
 */
void Task_Electricity_X(void *pvParameters);

#endif // VEHICLE_TASK_H