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
    { EVENT_AMBULANCE, 3, "Severe traffic accident" },
    { EVENT_AMBULANCE, 3, "Heart attack patient" },
    { EVENT_AMBULANCE, 2, "Minor accident" },
    { EVENT_AMBULANCE, 1, "Minor injury" },
    { EVENT_AMBULANCE, 3, "Emergency in public building" },

    /* Police */
    { EVENT_POLICE, 2, "Illegal gathering" },
    { EVENT_POLICE, 3, "Home burglary" },
    { EVENT_POLICE, 1, "Traffic violation" },
    { EVENT_POLICE, 3, "Violence incident" },
    { EVENT_POLICE, 2, "City emergency assistance" },

    /* Fire */
    { EVENT_FIRE_DEPARTMENT, 3, "Restaurant fire" },
    { EVENT_FIRE_DEPARTMENT, 3, "Residential house fire" },
    { EVENT_FIRE_DEPARTMENT, 2, "Vehicle fire" },
    { EVENT_FIRE_DEPARTMENT, 2, "Suspicious smoke" },
    { EVENT_FIRE_DEPARTMENT, 1, "Open field fire" },

    /* Maintenance */
    { EVENT_MAINTENANCE, 2, "Sidewalk repair" },
    { EVENT_MAINTENANCE, 3, "Water pipe leak" },
    { EVENT_MAINTENANCE, 1, "Routine public building maintenance" },
    { EVENT_MAINTENANCE, 2, "Dangerous sewer openings" },
    { EVENT_MAINTENANCE, 3, "Roof leak issue" },

    /* Waste */
    { EVENT_WASTE_COLLECTION, 2, "Full neighborhood bins" },
    { EVENT_WASTE_COLLECTION, 3, "Hazardous waste collection" },
    { EVENT_WASTE_COLLECTION, 1, "Regular bin collection" },
    { EVENT_WASTE_COLLECTION, 3, "Large public waste removal" },
    { EVENT_WASTE_COLLECTION, 2, "Uncollected paper bins from commerce" },

    /* Electricity */
    { EVENT_ELECTRICITY, 1, "Streetlight failure" },
    { EVENT_ELECTRICITY, 3, "Neighborhood power outage" },
    { EVENT_ELECTRICITY, 2, "Power off in public building" },
    { EVENT_ELECTRICITY, 3, "Overload in power network" },
    { EVENT_ELECTRICITY, 2, "Traffic light signaling failure" },
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

        /* Generate a random location using snprintf */
        snprintf(xNewEvent.location, sizeof(xNewEvent.location),
                 "Street %u", (unsigned)(rand() % 100U)); // Random street number between 0 and 99

        TickType_t now = xTaskGetTickCount(); // Get current tick count
        xNewEvent.timestampStart = (uint32_t)now; // Set start timestamp

        Db_InsertEventPending(&xNewEvent); // Insert the new event into the database
        
        /* Print the generated event details */
        printf("[Server] Generated: ID=%u Type=%d EventDetail='%s' Location='%s' Time=%lusec\n",
               (unsigned)xNewEvent.eventID,
               (int)xNewEvent.type,
               xNewEvent.event_detail,
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

