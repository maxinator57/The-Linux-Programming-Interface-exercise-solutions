#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


enum { N_MICROSECONDS_IN_SECOND = 1000000 };

struct timeval GetCurrentTime() {
    struct timeval currentTime = {};
    gettimeofday(&currentTime, NULL);
    return currentTime;
}


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

    // Send datagrams to the server in an infinite loop
    char datagram[DATAGRAM_MAX_SIZE + 1] = {};
    int datagramNumber = 0;
    for (char letter = 'a';; letter = (letter == 'z' ? 'a' : letter + 1), ++datagramNumber) {
        // Fill in the datagram to be sent
        for (int i = 0; i != DATAGRAM_MAX_SIZE; ++i) {
            datagram[i] = letter;
        }
        // Mark the current time before sending the datagram
        const struct timeval timeBeforeSending = GetCurrentTime();

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
                "when sending datagram number %d (\"%s\")\n",
                datagramNumber,
                datagram
            );
            exit(EXIT_FAILURE);
        }
        // Mark the time after sending the datagram
        const struct timeval timeAfterSending = GetCurrentTime();
        // Print the time that was taken by sending the current datagram
        const suseconds_t nMicrosecondsElapsed =
            ((suseconds_t) timeAfterSending.tv_sec - timeBeforeSending.tv_sec) * N_MICROSECONDS_IN_SECOND
            + timeAfterSending.tv_usec - timeBeforeSending.tv_usec;
        const time_t nFullSecondsElapsed = (time_t) (nMicrosecondsElapsed / N_MICROSECONDS_IN_SECOND);
        const suseconds_t nRemainingMicroseconds = nMicrosecondsElapsed % N_MICROSECONDS_IN_SECOND;
        printf(
            "Successfully sent datagram number %d (\"%s\") to the server, "
            "which took %ld seconds and %ld microseconds.\n",
            datagramNumber,
            datagram,
            nFullSecondsElapsed,
            nRemainingMicroseconds
        );
    }

    // Close the client file descriptor
    close(cfd);
}
