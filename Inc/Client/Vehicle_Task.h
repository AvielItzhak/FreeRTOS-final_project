/**
 * @file Vehicle_Task.h
 * @brief 
 *
 * @attention
 */

#ifndef VEHICLE_TASK_H
#define VEHICLE_TASK_H

/**
 * @brief This task to simulates Vehicle <-> Dispatcher communication.
 *        Independent Task for vehicle; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @attention This task is for testing purposes only.
 * @param pvParameters Not used pointer.
 */
void Task_TestVehicle(void *pvParameters);

/**
 * @brief This task to simulates Ambulance <-> Dispatcher communication.
 *        Independent Task for ambulance; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Ambulance_X(void *pvParameters);

/**
 * @brief This task to simulates Police <-> Dispatcher communication.
 *        Independent Task for police; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Police_X(void *pvParameters);

/**
 * @brief This task to simulates Fire Department <-> Dispatcher communication.
 *        Independent Task for fire department; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Fire_X(void *pvParameters);

/**
 * @brief This task to simulates Maintenance <-> Dispatcher communication.
 *        Independent Task for maintenance; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Maintenance_X(void *pvParameters);

/**
 * @brief This task to simulates Waste Collection <-> Dispatcher communication.
 *        Independent Task for waste collection; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Waste_X(void *pvParameters);

/**
 * @brief This task to simulates Electricity Department <-> Dispatcher communication.
 *        Independent Task for electricity services; pulls event from Dispatcher Queue, handles it, sends completion message to Client-UDP-TX Queue.
 * @attention This task priority is set to be lower than Dispatcher task priority - Low Level.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_Electricity_X(void *pvParameters);

#endif // VEHICLE_TASK_H