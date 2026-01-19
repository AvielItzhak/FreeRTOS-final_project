/**
 * @file Vehicle_Task.h
 * @brief 
 *
 * @attention
 */

#ifndef VEHICLE_TASK_H
#define VEHICLE_TASK_H

/**
 * @brief This task to simulates vehicle Dispatcher communication.
 *        Independent Task for vehicle; pulls event from Dispatcher Queue, handles it, sends completion to Client-UDP-TX Queue.
 * 
 * @param pvParameters Not used pointer.
 */
void Task_TestVehicle(void *pvParameters);

#endif // VEHICLE_TASK_H