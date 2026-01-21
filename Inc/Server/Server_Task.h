#ifndef SERVER_TASK_H
#define SERVER_TASK_H

#include "Shared_Configuration.h"

#define MAX_PRIORITY_LEVEL 3U // (1=Low, 2=Medium, 3=High)
#define EVENT_GENERATION_INTERVAL_MS 5000U

/* Event Catalog Item Structure - Fully describes an event */
typedef struct {
    EventType_t  type;       // Department
    uint8_t      priority;   // Derived priority for this event info
    const char  *detail;       // Event detail text
} EventCatalogItem_t;

/* Event Catalog Array and Count - Used in the event generator */
extern const EventCatalogItem_t eventCatalog[]; // Array of event catalog items
extern const uint32_t eventCatalogCount; // Number of items in the event catalog


/**
 * @brief Task function that generates random emergency events and stores them in the SQLite database.
 * 
 * @param pvParameters (currently unused)
 */
void Task_EventGenerator(void *pvParameters); 

#endif