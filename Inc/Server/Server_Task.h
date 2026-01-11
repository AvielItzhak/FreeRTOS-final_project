#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "Shared_Configuration.h"

#define MAX_PRIORITY_LEVEL 3U



void vServerEventGen(void *pvParameters); // Task to generate random emergency events every 5 seconds

#endif