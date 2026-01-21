/**
 * @file Server_Task.c
 * @brief This file contains the implementation of the server task that generates random emergency events
 *        and stores them in an SQLite database.
 * @attention This module implements the server task that generates random emergency events
 *            and stores them in an SQLite database.
 * @attention The event catalog is defined here as a constant array.
 */

#include "Server/Server_Task.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"

#include "Server/DataBase.h" // Database functions

#include "Server/Server_Task.h"


/* Event Catalog Array - Predefined events for random selection */
const EventCatalogItem_t eventCatalog[] = {

    /* Ambulance */
    { EVENT_AMBULANCE, HIGH_EVENT_PRIORITY_LEVEL, "Severe traffic accident", 60},
    { EVENT_AMBULANCE, HIGH_EVENT_PRIORITY_LEVEL, "Heart attack patient", 40},
    { EVENT_AMBULANCE, MEDIUM_EVENT_PRIORITY_LEVEL, "Minor accident", 10},
    { EVENT_AMBULANCE, LOW_EVENT_PRIORITY_LEVEL, "Minor injury", 5},
    { EVENT_AMBULANCE, HIGH_EVENT_PRIORITY_LEVEL, "Emergency in public building", 25},
    /* Police */
    { EVENT_POLICE, MEDIUM_EVENT_PRIORITY_LEVEL, "Illegal gathering", 15},
    { EVENT_POLICE, HIGH_EVENT_PRIORITY_LEVEL, "Home burglary", 20},
    { EVENT_POLICE, LOW_EVENT_PRIORITY_LEVEL, "Traffic violation", 5},
    { EVENT_POLICE, HIGH_EVENT_PRIORITY_LEVEL, "Violence incident", 25},
    { EVENT_POLICE, MEDIUM_EVENT_PRIORITY_LEVEL, "City emergency assistance", 20},
    /* Fire */
    { EVENT_FIRE_DEPARTMENT, HIGH_EVENT_PRIORITY_LEVEL, "Restaurant fire", 50},
    { EVENT_FIRE_DEPARTMENT, HIGH_EVENT_PRIORITY_LEVEL, "Residential house fire", 70},
    { EVENT_FIRE_DEPARTMENT, MEDIUM_EVENT_PRIORITY_LEVEL, "Vehicle fire", 15},
    { EVENT_FIRE_DEPARTMENT, MEDIUM_EVENT_PRIORITY_LEVEL, "Suspicious smoke", 10},
    { EVENT_FIRE_DEPARTMENT, LOW_EVENT_PRIORITY_LEVEL, "Open field fire", 5},

    /* Maintenance */
    { EVENT_MAINTENANCE, MEDIUM_EVENT_PRIORITY_LEVEL, "Sidewalk repair", 60},
    { EVENT_MAINTENANCE, HIGH_EVENT_PRIORITY_LEVEL, "Water pipe leak", 50},
    { EVENT_MAINTENANCE, LOW_EVENT_PRIORITY_LEVEL, "Routine public building maintenance", 10},
    { EVENT_MAINTENANCE, MEDIUM_EVENT_PRIORITY_LEVEL, "Dangerous sewer openings", 15},
    { EVENT_MAINTENANCE, HIGH_EVENT_PRIORITY_LEVEL, "Roof leak issue", 25},
    /* Waste */
    { EVENT_WASTE_COLLECTION, MEDIUM_EVENT_PRIORITY_LEVEL, "Full neighborhood bins", 60},
    { EVENT_WASTE_COLLECTION, HIGH_EVENT_PRIORITY_LEVEL, "Hazardous waste collection", 90},
    { EVENT_WASTE_COLLECTION, LOW_EVENT_PRIORITY_LEVEL, "Regular bin collection", 15},
    { EVENT_WASTE_COLLECTION, HIGH_EVENT_PRIORITY_LEVEL, "Large public waste removal", 25},
    { EVENT_WASTE_COLLECTION, MEDIUM_EVENT_PRIORITY_LEVEL, "Uncollected paper bins from commerce", 15},

    /* Electricity */
    { EVENT_ELECTRICITY, LOW_EVENT_PRIORITY_LEVEL, "Streetlight failure", 15},
    { EVENT_ELECTRICITY, HIGH_EVENT_PRIORITY_LEVEL, "Neighborhood power outage", 30},
    { EVENT_ELECTRICITY, MEDIUM_EVENT_PRIORITY_LEVEL, "Power off in public building", 15},
    { EVENT_ELECTRICITY, HIGH_EVENT_PRIORITY_LEVEL, "Overload in power network", 60},
    { EVENT_ELECTRICITY, MEDIUM_EVENT_PRIORITY_LEVEL, "Traffic light signaling failure", 20},
};

const uint32_t eventCatalogCount = (uint32_t)(sizeof(eventCatalog)/sizeof(eventCatalog[0])); // Number of items in the event catalog




void Task_EventGenerator(void *pvParameters)
{
    (void)pvParameters;

    Db_Init();

    /* Seed rand() once so each RUN offers random results */
    static int seeded = 0; // Static variable to track if seeded
    if (!seeded) { // Only if Not seeded yet
        seeded = 1; // Mark as seeded
        srand((unsigned)time(NULL) ^ (unsigned)xTaskGetTickCount()); // Seed with time and tick count
    }

    uint32_t ulIDCounter = Db_GetNextEventId(); // Get starting event ID from DB

    printf("[Server] EventGen Started (next_id=%u)\n", (unsigned)ulIDCounter);
    vTaskDelay(pdMS_TO_TICKS(Long_Delay_MS)); // Initial delay before starting event generation

    /* Main Loop to Generate events and insert them into the database */
    for (;;) {
        EmergencyEvent_t xNewEvent; // New event structure

        xNewEvent.eventID  = ulIDCounter++; // Assign and increment event ID
        xNewEvent.type     = (EventType_t)(rand() % EVENT_MAX); // Random event type with the range of 1 to EVENT_MAX -1

        /* Assign priority based on generated event catalog */
        const EventCatalogItem_t *randCatalogItem = &eventCatalog[rand() % eventCatalogCount]; // Randomly select an event from the catalog
        xNewEvent.type = randCatalogItem->type; // Set event type from catalog
        xNewEvent.priority = randCatalogItem->priority; // Set priority from catalog
        snprintf(xNewEvent.event_detail, sizeof(xNewEvent.event_detail), "%s", randCatalogItem->detail); // Set event detail from catalog
        xNewEvent.delayFactor = randCatalogItem->delayFactor; // Set delay factor from catalog

        /* Generate a random location using snprintf */
        snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                 "Street %u", (unsigned)(rand() % 100U)); // Random street number between 0 and 99

        TickType_t now = xTaskGetTickCount(); // Get current tick count
        xNewEvent.timestampStart = (uint32_t)now; // Set start timestamp

        Db_InsertEventPending(&xNewEvent); // Insert the new event into the database
        
        /* Print the generated event details */
        printf("[Server] Generated: ID=%u Type=%d EventDetail='%s' handleTime=%lusec Location='%s' Time=%lusec\n",
               (unsigned)xNewEvent.eventID,
               (int)xNewEvent.type,
               xNewEvent.event_detail,
               (unsigned long)(xNewEvent.delayFactor * baseEventHandling_Delay_MS / 1000U), // Handling time in seconds
               xNewEvent.location,
               (unsigned long)(now / 1000U));
        

        /* Send the event to the UDP TX queue */
        if (xQueueSend(handle_serverUDPTxQ, &xNewEvent, pdMS_TO_TICKS(Medium_Delay_MS)) != pdPASS) {
            printf("[Server] WARN: UDP-TX queue full, drop event id=%u\n", (unsigned)xNewEvent.eventID);
        }
        printf("[Server] Sent to queue event id=%u to UDP-TX\n", (unsigned)xNewEvent.eventID);


        vTaskDelay(pdMS_TO_TICKS(EVENT_GENERATION_INTERVAL_MS)); // Delay before generating the next event
    }

    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}

