#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>


struct sockaddr_un CreateUnixDomainDatagramSocket(const char* sockPath) {
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    strncpy(
        // Create socket in the Linux Abstract Socket Namespace
        /* dst */ &socketAddr.sun_path[1],
        /* src */ sockPath,
        /* len */ sizeof(socketAddr.sun_path) - 2
    );
    return socketAddr;
}


int main() {
    // Create the first UNIX domain datagram socket address struct.
    const struct sockaddr_un firstSocketAddr =
        CreateUnixDomainDatagramSocket("first_socket");
    // Create the file descriptor for the first socket.
    const int firstSocketFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (firstSocketFd == -1) {
        perror("socket() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }
    // Bind the first socket file descriptor to the first socket address.
    if (bind(
            firstSocketFd,
            (const struct sockaddr*) &firstSocketAddr,
            sizeof(firstSocketAddr)
    ) == -1) {
        perror("bind(firstSocketFd) syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create the second UNIX domain datagram socket address struct.
    const struct sockaddr_un secondSocketAddr =
        CreateUnixDomainDatagramSocket("second_socket");
    // Create the file descriptor for the second socket.
    const int secondSocketFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (secondSocketFd == -1) {
        perror("socket() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }
    // Bind the second socket file descriptor to the second socket address.
    if (bind(
            secondSocketFd,
            (const struct sockaddr*) &secondSocketAddr,
            sizeof(secondSocketAddr)
    ) == -1) {
        perror("bind(secondSocketFd) syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Connect the first socket to the second socket.
    if (connect(
        firstSocketFd,
        (const struct sockaddr*) &secondSocketAddr,
        sizeof(secondSocketAddr)
    ) == -1) {
        perror("connect() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create the third UNIX domain datagram socket.
    const int thirdSocketFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (thirdSocketFd == -1) {
        perror("socket() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Try to send a datagram from the third socket to the first socket.
    const char datagram = 'x';
    if (sendto(
            thirdSocketFd,
            &datagram,
            1,
            0,
            (const struct sockaddr*) &firstSocketAddr,
            sizeof(firstSocketAddr)
    ) == -1) {
        perror(
            "sendto() syscall from third socket to the "
            "first socket failed with the following error"
        );
        const char* errorCode = strerrorname_np(errno);
        printf("Error code: %d (%s)\n", errno, errorCode);
    }
}