/**
 * @file CLient_UDP.h
 * @brief This header file declares the UDP CLient initialization and socket retrieval functions.
 *        It is used to set up a UDP CLient that listens for incoming messages on a predefined port.
 * 
 * @attention This file is part of the CLient module.
 */

#ifndef CLient_UDP_H
#define CLient_UDP_H

#include <stdint.h>

/**
 * @brief Initializes the UDP CLient by creating and binding a socket to the predefined CLient port.
 *        It is used by the CLient tasks to communicate with the server over UDP on the same PORT.
 * 
 * @attention This function should be called once before using the CLient socket.
 * @return int Returns 0 on success, or a negative error code (9X) on failure.
 */
int ClientUDP_Init(void);

/**
 * @brief Retrieves the UDP CLient socket file descriptor.
 *        It is used by CLient tasks to know the CLient socket.
 * 
 * @attention This function should be called after ClientUDP_Init().
 * @return int The socket file descriptor, or -1 if the socket is not initialized.
 */
int ClientUDP_GetSocket(void);

/**
 * @brief This task handles receiving EmergencyEvent_t messages via UDP from the server.
 *        It receives messages from the network and sends them to a queue.
 * 
 * @attention This task should be created after ClientUDP_Init() is called.
 * @param pvParameters - Not used 
 */
void vClientUDPRxTask(void *pvParameters);

/**
 * @brief This task handles transmitting EmergencyEvent_t messages via UDP from the client to the server.
 *        It receives messages from a queue and sends them over the network.
 * 
 * @attention This task should be created after ClientUDP_Init() is called.
 * @param pvParameters - Not used
 */
void vClientUDPTxTask(void *pvParameters);

/**
 * @brief This task implements an echo mechanism: it receives EmergencyEvent_t messages from a queue,
 *        generates corresponding CompletionMsg_t messages, and sends them to the TX queue.
 *        This is a simple echo task for demonstration purposes.
 * 
 * @attention It should be created after ClientUDP_Init() is called.
 * @param pvParameters - Not used
 */
void vClientEchoTask(void *pvParameters);


#endif // CLient_UDP_H
