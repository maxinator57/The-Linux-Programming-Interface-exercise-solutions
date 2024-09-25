#include "common.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    // Create server socket file descriptor
    const int sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == - 1) {
        perror("An error occurred in \"socket\" system call");
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

    // Remove the server socket file if it exists 
    if (remove(SERVER_SOCKET_PATH) == - 1 && errno != ENOENT) {
        perror("An error occurred when trying to remove the server socket file");
        printf("at path %s", SERVER_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    // Bind the server socket file descriptor to the address
    if (bind(sfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
        perror(
            "An error occurred when trying to bind the server socket"
            " file descriptor"
        );
        printf("to the unix domain address %s", SERVER_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    // Receive datagramns in a loop and sleep for 1 second after
    // receiving each datagram and printing its contents to stdout
    char datagram[DATAGRAM_MAX_SIZE + 1] = {};
    enum {N_DATAGRAMS_TO_RECEIVE = 300};
    for (int i = 0; i != N_DATAGRAMS_TO_RECEIVE; ++i) {
        // Receive the next datagram
        const int numBytesRead = recv(sfd, datagram, DATAGRAM_MAX_SIZE, 0);
        if (numBytesRead == -1) {
            perror("An error occured in \"recvfrom\" system call");
            exit(EXIT_FAILURE);
        }
        // Print the datagram to stdout
        datagram[numBytesRead] = 0;
        printf(
            "Received datagram number %d (\"%s\") from the client, sleeping for 1 second...\n",
            i,
            datagram
        );
        // Sleep for 1 second
        sleep(1);
    }
}
