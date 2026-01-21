/**
 * @file Dispatcher_Task.h
 * @brief Declares the Dispatcher task function.
 * 
 * @attention
 */

#ifndef DISPATCHER_TASK_H
#define DISPATCHER_TASK_H

/**
 * @brief Receives events from UDP RX queue and forwards them to the appropriate department queue based on event type.
 * @attention This task priority is set to be higher than department tasks - Medium Level.
 * @param pvParameters Not used pointer.
 */
void Task_Dispatcher(void *pvParameters);

#endif
