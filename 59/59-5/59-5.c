#define _GNU_SOURCE


#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


struct sockaddr_in6 CreateIpV6LocalhostSocketAddr(
    const in_port_t port /* in host byte order */
) {
    struct sockaddr_in6 socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin6_family = AF_INET6;
    socketAddr.sin6_port = htons(port);
    socketAddr.sin6_addr = in6addr_loopback;
    return socketAddr;
}


int main() {
    // Create `sockaddr_in` structs with addresses
    // for the first and second sockets.
    const struct sockaddr_in6 firstSocketAddr = CreateIpV6LocalhostSocketAddr(40000),
                             secondSocketAddr = CreateIpV6LocalhostSocketAddr(50000),
                              thirdSocketAddr = CreateIpV6LocalhostSocketAddr(60000);

    // Create the file descriptor for the first socket.
    const int firstSocketFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (firstSocketFd == -1) {
        perror("the first socket() syscall failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }
    // Bind the first socket to the first socket address.
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
    // Bind the third socket file descriptor to the third socket address.
    if (bind(
        thirdSocketFd,
        (const struct sockaddr*) &thirdSocketAddr,
        sizeof(thirdSocketAddr)
    ) == -1) {
        perror("bind() syscall for the third socket failed with the following error");
        printf("(error code: %s)\n", strerrorname_np(errno));
        exit(EXIT_FAILURE);
    }

    // Define a datagram.
    const char datagram = 'x';

    // Try to send a datagram from the second socket to the first socket.
    {
        const ssize_t nBytesSent = sendto(
            secondSocketFd,
            &datagram,
            1,
            0,
            (const struct sockaddr*) &firstSocketAddr,
            sizeof(firstSocketAddr)
        );
        if (nBytesSent == -1) {
            perror(
                "sendto() syscall from the third socket to the first "
                "socket failed with the following error"
            );
            printf("(error code: %s)\n", strerrorname_np(errno));
        } else {
            printf(
                "sendto() syscall successfully sent %ld byte(s) "
                "from the third socket to the first socket\n",
                nBytesSent
            );
        }
    }

    // Try to send a datagram from the third socket to the first socket.
    {
        const ssize_t nBytesSent = sendto(
            thirdSocketFd,
            &datagram,
            1,
            0,
            (const struct sockaddr*) &firstSocketAddr,
            sizeof(firstSocketAddr)
        );
        if (nBytesSent == -1) {
            perror(
                "sendto() syscall from the third socket to the first "
                "socket failed with the following error"
            );
            printf("(error code: %s)\n", strerrorname_np(errno));
        } else {
            printf(
                "sendto() syscall successfully sent %ld byte(s) "
                "from the third socket to the first socket\n",
                nBytesSent
            );
        }
    }

    // Try to receive the datagrams from the second and third socket on the first socket.
    struct sockaddr_in6 senderAddr;
    socklen_t senderAddrSize = sizeof(senderAddr);
    char incomingDatagram;
    for (int i = 0; i != 2; ++i) { // try to two datagrams
        const ssize_t nBytesReceived = recvfrom(
            firstSocketFd,
            &incomingDatagram,
            1,
            MSG_DONTWAIT, // make this call non-blocking
            (struct sockaddr*) &senderAddr,
            &senderAddrSize
        );
        if (nBytesReceived == -1) {
            perror(
                "recvfrom() syscall on the first socket "
                "failed with the following error"
            );
            printf("(error code: %s)\n", strerrorname_np(errno));
            exit(EXIT_FAILURE);
        } else {
            assert(senderAddr.sin6_family == AF_INET6);
            printf(
                "recvfrom() syscall on the first socket successfully received %ld byte(s).\n"
                "Sender address:\n"
                "    Family: %d\n"
                "    Port: %d\n"
                "    IP address: ",
                nBytesReceived,
                senderAddr.sin6_family,
                ntohs(senderAddr.sin6_port)
            );
            const uint8_t* senderIpV6Addr = (const uint8_t *) &senderAddr.sin6_addr;
            printf("%u", senderIpV6Addr[0]);
            for (size_t i = 1; i < sizeof(senderAddr.sin6_addr); ++i) {
                printf(":%u", senderIpV6Addr[i]);
            }
            puts("");
        }
    }
}
