/**
 * @file Client_UDP.c
 * @brief Implementation of UDP client functionality.
 * @attention This file is part of the Client module.
 */

#include "Shared_Configuration.h"
#include "Client/Client_UDP.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int ClientSock = -1; // Defining general Client UDP socket variable
static struct sockaddr_in serverAddr; // Defining general Server address structure


/* Initialize the UDP client socket */
int ClientUDP_Init(void)
{
    struct sockaddr_in clientAddr; // Client address structure

    /* Create a UDP socket */
    ClientSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (ClientSock < 0) {
        perror("Client socket");
        return 97;
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port   = htons(UDP_CLIENT_PORT); 
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    /* Bind the socket to the client address */
    if (bind(ClientSock, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0) {
        perror("Client bind");
        close(ClientSock);
        ClientSock = -1;
        return 96;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(UDP_SERVER_PORT);
    inet_pton(AF_INET, UDP_IP_Addr, &serverAddr.sin_addr);

    printf("[Client][UDP] Listening on port %d\n", UDP_CLIENT_PORT);
    return 0;
}

/* Retrieve the UDP client socket */
int ClientUDP_GetSocket(void)
{
    return ClientSock; // Return the client socket descriptor
}

/* Receive UDP packets and send to RX queue */
void vClientUDPRxTask(void *pvParameters)
{
    (void)pvParameters;

    /* Check if the client socket is initialized */
    if (ClientSock < 0) {
        printf("[Client][UDP-RX] ERROR: ClientUDP_Init() not called\n");
        vTaskDelete(NULL);
    }

    printf("[Client][UDP-RX] Started\n");

    /* Main loop to receive UDP packets from Server */
    for (;;) {
        EmergencyEvent_t event;
        ssize_t n = recvfrom(ClientSock, &event, sizeof(event), 0, NULL, NULL);

        /* Check if the received data matches the expected size */
        if (n == (ssize_t)sizeof(event)) {
            (void)xQueueSend(handle_clientUDPRxQ, &event, 0);
            printf("[Client][UDP-RX] Got event id=%u\n", (unsigned)event.eventID);
        } else {
            vTaskDelay(pdMS_TO_TICKS(10)); // No valid message, short delay to avoid busy waiting on error
        }
    }

    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}

/* Send UDP packets from TX queue to Server */
void vClientUDPTxTask(void *pvParameters)
{
    (void)pvParameters;

    /* Check if the client socket is initialized */
    if (ClientSock < 0) {
        printf("[Client][UDP-TX] ERROR: ClientUDP_Init() not called\n");
        vTaskDelete(NULL);
    }

    printf("[Client][UDP-TX] Started\n");

    /* Main loop to send UDP packets to Server */
    for (;;) {
        CompletionMsg_t msg; // Completion message to send

        /* Receive completion message from TX queue */
        if (xQueueReceive(handle_clientUDPTxQ, &msg, portMAX_DELAY) == pdPASS) {
            (void)sendto(ClientSock, &msg, sizeof(msg), 0,
                         (struct sockaddr*)&serverAddr, sizeof(serverAddr));

            printf("[Client][UDP-TX] Sent completion for event id=%u\n",
                   (unsigned)msg.eventID);
        }
    }

    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}

/* Echo task used for testing UDP Transmission */
void vClientEchoTask(void *pvParameters)
{
    (void)pvParameters;

    printf("[Client][ECHO] Started\n");

    /* Main loop for echoing received events */
    for (;;) {
        EmergencyEvent_t event;

        /* Receive an emergency event from the RX queue */
        if (xQueueReceive(handle_clientUDPRxQ, &event, portMAX_DELAY) == pdPASS) {
            CompletionMsg_t ComMSG; // Completion message to send back
            memset(&ComMSG, 0, sizeof(ComMSG));

            /* Initialize the completion message */
            ComMSG.eventID = event.eventID; // Same event ID as received
            snprintf(ComMSG.handledBy, sizeof(ComMSG.handledBy), "ECHO"); // Mark as handled by ECHO
            ComMSG.timestampEnd = (uint32_t)xTaskGetTickCount(); // Current tick count as end timestamp
            ComMSG.status = (rand() % 100 < 50) ? 1 : 0; // Creating 50% success using random number


            (void)xQueueSend(handle_clientUDPTxQ, &ComMSG, 0); // Send to TX queue
        }

        printf("[Client][ECHO] Processed event id=%u\n", (unsigned)event.eventID);
    }

    vTaskDelete(NULL); // Delete and free resources - Should never reach here
}