#ifndef DISPATCHER_TASK_H
#define DISPATCHER_TASK_H

#include "Shared_Configuration.h"

extern QueueHandle_t xIncomingEventsQueue;
extern QueueHandle_t xDeptQueues[EVENT_MAX];

void vDispatcherTask(void *pvParameters);

#endif // DISPATCHER_TASK_H