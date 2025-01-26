#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pduUtil.h"
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUF 1024


int sendPDU(int clientSocket, uint8_t * dataBuffer, int lengthOfData) {
    // check if message is too long
    if (lengthOfData > MAXBUF) {
        perror("message too long and can't find max buffer of 1024");
        return -1;
    }

    uint8_t pduBuffer[MAXBUF + 2];  // To store the entire PDU
    int pduLengthField = htons(lengthOfData);  // The length field
    memcpy(pduBuffer, &pduLengthField, sizeof(uint16_t));  // Adding the length to the front of the buffer 
    memcpy(pduBuffer + 2, dataBuffer, lengthOfData * sizeof(uint8_t));  // Adding the entire message
    int bytesSent;
    if ((bytesSent = send(clientSocket, pduBuffer, lengthOfData + 2, 0)) < 0)
	{
        perror("send call\n");
        return -1;
     }
	 
    return lengthOfData;
}
int recvPDU(int socketNumber, uint8_t * dataBuffer, int bufferSize) {
    uint8_t lenghtBuffer[2]; // Buffer for the length field in first recv
    int bytesReceived = recv(socketNumber, lenghtBuffer, 2, MSG_WAITALL); // recv only 2 bytes
    if (bytesReceived == 0)  
    {
            printf("Got a length of zero from first PDU recv, close connection\n");
            return 0; // close connection
    }
    else if (bytesReceived < 0)  
    {
            perror("First PDU recv got an error\n");
            return -1;
    }

    int messageLen = lenghtBuffer[1]; // Get length of meesage
    printf("Got message length of %d for PDU\n", messageLen);
    bytesReceived = recv(socketNumber, dataBuffer, messageLen, MSG_WAITALL); // recv the message length from stream
    if (bytesReceived < 0)  
    {
            perror("Second PDU recv got an error\n");
            return -1;
    }
    return bytesReceived;
}