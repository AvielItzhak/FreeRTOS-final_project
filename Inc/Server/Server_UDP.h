/**
 * @file Server_UDP.h
 * @brief This header file declares the UDP server initialization and socket retrieval functions.
 *        It is used to set up a UDP server that listens for incoming messages on a predefined port.
 * 
 * @attention This file is part of the server module.
 */

#ifndef SERVER_UDP_H
#define SERVER_UDP_H

#include <stdint.h>

/**
 * @brief Initializes the UDP server by creating and binding a socket to the predefined server port.
 *        It is used by the server tasks to communicate with the client over UDP on the same PORT.
 * 
 * @attention This function should be called once before using the server socket.
 * @return int Returns 0 on success, or a negative error code (9X) on failure.
 */
int ServerUDP_Init(void);

/**
 * @brief Retrieves the UDP server socket file descriptor.
 *        It is used by server tasks to know the server socket.
 * @return int The socket file descriptor, or -1 if the socket is not initialized.
 */
int ServerUDP_GetSocket(void);

/**
 * @brief This task handles transmitting EmergencyEvent_t messages via UDP from the server to the client.
 *        It receives messages from a queue and sends them over the network.
 * 
 * @attention This task should be created after ServerUDP_Init() is called.
 * @param pvParameters - Not used
 */
void vServerUDPTxTask(void *pvParameters);

/**
 * @brief This task handles receiving CompletionMsg_t messages via UDP from the client.
 *        It receives messages from the network and sends them to a queue.
 * 
 * @attention This task should be created after ServerUDP_Init() is called.
 * @param pvParameters - Not used
 */
void vServerUDPRxTask(void *pvParameters);


#endif // SERVER_UDP_H