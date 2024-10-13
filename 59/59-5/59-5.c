#define _GNU_SOURCE

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


struct sockaddr_in6 CreateIpV6LocalhostSocketAddr(const in_port_t port) {
    struct sockaddr_in6 socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin6_family = AF_INET6;
    socketAddr.sin6_port = port;
    socketAddr.sin6_addr = in6addr_loopback;
    return socketAddr;
}


int main() {
    // Create `sockaddr_in` structs with addresses
    // for the first and second sockets.
    const struct sockaddr_in6 firstSocketAddr = CreateIpV6LocalhostSocketAddr(40000),
                             secondSocketAddr = CreateIpV6LocalhostSocketAddr(50000);

    // Create the file descriptor for the first socket.
    const int firstSocketFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (firstSocketFd == -1) {
        perror("the first socket() syscall failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }
    // Bind the first socket file descriptor to the first socket address.
    if (bind(
        firstSocketFd,
        (const struct sockaddr*) &firstSocketAddr,
        sizeof(firstSocketAddr)
    ) == -1) {
        perror("bind() syscall for the first socket failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }

    // Create the file descriptor for the second socket.
    const int secondSocketFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (secondSocketFd == -1) {
        perror("the second socket() syscall failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }
    // Bind the second socket file descriptor to the second socket address.
    if (bind(
        secondSocketFd,
        (const struct sockaddr*) &secondSocketAddr,
        sizeof(secondSocketAddr)
    ) == -1) {
        perror("bind() syscall for the second socket failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }

    // Connect the first socket to the second socket.
    if (connect(
        firstSocketFd,
        (const struct sockaddr*) &secondSocketAddr,
        sizeof(secondSocketAddr)
    ) == -1) {
        perror(
            "connect() from the first socket to the second "
            "socket failed with the following error"
        );
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }

    // Create the file descriptor for the third socket.
    const int thirdSocketFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (thirdSocketFd == -1) {
        perror("the third socket() syscall failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
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
            "sendto() from the third socket to the first "
            "socket failed with the following error"
        );
        printf("(error code: %s)\n", strerrorname_np(errno));
    }
}
