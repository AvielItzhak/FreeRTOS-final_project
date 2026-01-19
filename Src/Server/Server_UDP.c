/**
 * @file Server_UDP.c
 * @brief Implementation of UDP server functionality.
 * @attention This file is part of the Server module.
 */

#include "Shared_Configuration.h"
#include "Server/Server_UDP.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Server/DataBase.h" // Database functions

static int serverSock = -1; // Defining Server UDP socket variable


/* Initialize UDP server socket once */
int ServerUDP_Init(void)
{
    struct sockaddr_in addr; // Address structure
    memset(&addr, 0, sizeof(addr));

    /* Create a UDP socket */
    serverSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSock < 0) {
        perror("[Server][UDP] socket");
        return -91;
    }

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(UDP_SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket to the specified port and address */
    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[Server][UDP] bind");
        close(serverSock);
        serverSock = -1;
        return -92;
    }

    printf("[Server][UDP] Listening on %d\n", UDP_SERVER_PORT);
    return 0;
}

/* Retrieve the UDP server socket */
int ServerUDP_GetSocket(void)
{
    return serverSock; // Return the server socket descriptor
}

/* Send UDP messages from Server Task */
void vServerUDPTxTask(void *pvParameters)
{
    (void)pvParameters;

    /* Create a UDP socket for transmission */
    int txSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (txSock < 0) {
        printf("[Server][UDP-TX] socket failed: %s\n", strerror(errno));
        vTaskDelete(NULL);
    }

    /* Define destination address */
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(UDP_CLIENT_PORT);
    inet_pton(AF_INET, UDP_IP_Addr, &dest.sin_addr);

    printf("[Server][UDP-TX] Started\n");

    /* Main transmission loop */
    for (;;) {
        EmergencyEvent_t event;

        if (xQueueReceive(handle_serverUDPTxQ, &event, portMAX_DELAY) == pdPASS) { // Received from queue only if available
            ssize_t s = sendto(txSock, &event, sizeof(event), 0, (struct sockaddr*)&dest, sizeof(dest));
            if (s != (ssize_t)sizeof(event)) { // sendto error
                printf("[Server][UDP-TX] sendto failed: %s\n", strerror(errno));
            } else { // Successful send
                printf("[Server][UDP-TX] Sent event id=%u\n", (unsigned)event.eventID);
            }
        }
    }

    close(txSock); // Close the transmission socket
    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}

/* Receive UDP messages from Client */
void vServerUDPRxTask(void *pvParameters)
{
    (void)pvParameters;

    /* Get the UDP socket */
    int rxSock = ServerUDP_GetSocket();
    if (rxSock < 0) {
        printf("[Server][UDP-RX] ERROR: ServerUDP_Init was not called\n");
        vTaskDelete(NULL);
    }

    printf("[Server][UDP-RX] Started\n");

    /* Main Client to Server loop */
    for (;;) {
        CompletionMsg_t msg;
        struct sockaddr_in sender;
        socklen_t senderLen = sizeof(sender);

        ssize_t n = recvfrom(rxSock, &msg, sizeof(msg), 0,
                             (struct sockaddr*)&sender, &senderLen);
                             
        /* Check if the received data matches the expected size */
        if (n == (ssize_t)sizeof(msg)) {
            (void)xQueueSend(handle_serverUDPRxQ, &msg, 0);
            printf("[Server][UDP-RX] Received: id=%u by='%s' status=%u\n",
                   (unsigned)msg.eventID, msg.handledBy, (unsigned)msg.status);
            
            /* Update DataBase */
            Db_UpdateEventCompletion(&msg);
            continue; // Continue to next iteration
        }

        /* Check for errors */
        if (n < 0) { 
            /* Importany to check errno for EINTR and EAGAIN & EWOULDBLOCK */
            if (errno == EINTR) { // Ignore EINTR (very common in FreeRTOS POSIX demo)
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) { // Ignore EAGAIN and EWOULDBLOCK
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }

            printf("[Server][UDP-RX] recvfrom failed: %s\n", strerror(errno));
            vTaskDelay(pdMS_TO_TICKS(10)); // Short delay to avoid busy waiting
            continue;
        }

        /* if n >= 0 but still invalid size */
        printf("[Server][UDP-RX] invalid datagram size=%ld\n", (long)n);
        vTaskDelay(pdMS_TO_TICKS(10)); // Short delay to avoid busy waiting
    }

    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}
