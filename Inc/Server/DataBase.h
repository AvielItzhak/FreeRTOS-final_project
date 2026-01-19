/**
 * @file DataBase.h
 * @brief SQLite database interface for emergency event logging.
 *        This module provides functions to initialize the database, insert events, and manage database connections.
 */

#ifndef DB_H
#define DB_H


#include <sqlite3.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "Shared_Configuration.h" // for EmergencyEvent_t


/**
 * @brief This function initializes the SQLite database and creates the necessary schema if it does not exist.
 * @attention This function uses a mutex to ensure thread-safe access to the database.
 * @attention This function should be called once before any other database operations.
 */
void Db_Init(void);

/**
 * @brief This module gets the next event id - Current DB Max ID num + 1
 * @attention This function uses a mutex to ensure thread-safe access to the database.
 * @attention This function checks for DB initialization.
 * @return Next event ID to use as uint32_t type.
 */
uint32_t Db_GetNextEventId(void);

/**
 * @brief Insert one event input as pending into the database.
 * @attention This function uses a mutex to ensure thread-safe access to the database.
 * @attention This function checks for DB initialization and valid input.
 * @param e - Pointer to EmergencyEvent_t structure containing event details.
 */
void Db_InsertEventPending(const EmergencyEvent_t *e);

/**
 * @brief Update an event's status in the database after receiving a completion message.
 * @attention This function uses a mutex to ensure thread-safe access to the database.
 * @attention This function checks for DB initialization and valid input.
 * @param msg - Pointer to CompletionMsg_t structure containing completion details.
 */
void Db_UpdateEventCompletion(const CompletionMsg_t *msg);

/**
 * @brief Close the database connection and release resources.
 * @attention This function uses a mutex to ensure thread-safe access to the database.
 * @attention This function checks for DB initialization.
 */
void Db_Close(void);

#endif // DB_H