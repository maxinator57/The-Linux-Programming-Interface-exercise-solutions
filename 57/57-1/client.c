#include "common.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>


int main() {
    // Create client socket file descriptor
    const int cfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (cfd == -1) {
        perror("An error occured in \"socket\" system call");
        exit(EXIT_FAILURE);
    }

    // Fill in the structure with the server socket address
    struct sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strncpy(
        serverAddr.sun_path,
        SERVER_SOCKET_PATH,
        sizeof(serverAddr.sun_path) - 1
    );

    // Send datagramns to the server in an infinite loop

    char datagram[DATAGRAM_MAX_SIZE + 1] = {};
    int datagramNumber = 0;
    for (char letter = 'a';; letter = (letter == 'z' ? 'a' : letter + 1), ++datagramNumber) {
        // Fill in the datagram to be sent
        for (int i = 0; i != DATAGRAM_MAX_SIZE; ++i) {
            datagram[i] = letter;
        }
        // Mark the current time before sending the datagram
        const time_t timeBeforeSending = time(NULL);
        // Send the datagram
        const int numBytesSent = sendto(
            cfd,
            datagram,
            DATAGRAM_MAX_SIZE,
            0,
            (struct sockaddr*) &serverAddr,
            sizeof(serverAddr)
        ); 
        // Check for errors from datagram sending
        if (numBytesSent != DATAGRAM_MAX_SIZE) {
            perror("An error occurred in \"sendto\" system call");
            printf(
                "when sending datagram number %d (\"%s\")",
                datagramNumber,
                datagram
            );
            exit(EXIT_FAILURE);
        }
        // Mark the time after sending the datagram
        const time_t timeAfterSending = time(NULL);
        // Print the time that was taken by sending the current datagram
        printf(
            "Successfully sent datagram number %d (\"%s\") to the server, "
            "which took %ld seconds.\n",
            datagramNumber,
            datagram,
            timeAfterSending - timeBeforeSending
        );
    }
}
