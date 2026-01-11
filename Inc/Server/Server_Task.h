#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "Shared_Configuration.h"

#define MAX_PRIORITY_LEVEL 3U
#define EVENT_GENERATION_INTERVAL_MS 5000U


/**
 * @brief Task function that generates random emergency events and stores them in the SQLite database.
 * 
 * @param pvParameters (currently unused)
 */
void vServerEventGen(void *pvParameters); 

#endif