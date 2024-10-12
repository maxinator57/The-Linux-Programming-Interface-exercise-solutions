#include "common.h"


int main() {
    // Create the struct with server socket address. 
    const struct sockaddr_un serverSocketAddr = GetServerSocketAddress();

    // Open the file descriptor for the client socket.
    const int clientSocketFd = socket(SOCKET_ADDRESS_FAMILY, SOCKET_TYPE, 0);
    if (clientSocketFd == -1) {
        perror("socket() system call failed with the following error");
        exit(EXIT_FAILURE); 
    }

    // Try to connect the client socket to the server socket.
    if (connect(
            clientSocketFd,
            (struct sockaddr*) &serverSocketAddr,
            sizeof(serverSocketAddr)
    ) == -1) {
        perror("connect() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create a buffer for storing input data from stdin.
    char buf[BUF_SIZE];

    // Start retransmitting bytes from stdin to the server socket.
    for (;;) {
        const ssize_t nBytesRead = read(STDIN_FILENO, buf, BUF_SIZE);
        if (nBytesRead == 0) { // the user has finished writing data to stdin
            break;
        } else if (nBytesRead == -1) {
            perror("read(clientPeerSocketFd) syscall failed with the following error");
            exit(EXIT_FAILURE);
        }
        // Try to send all the data received from the user to the server socket.
        if (write(clientSocketFd, buf, nBytesRead) != nBytesRead) {
            printf("Partial/failed write to client socket");
            exit(EXIT_FAILURE);
        }
    }
    // Try to close the client socket file descriptor
    if (close(clientSocketFd) == -1) {
        perror("close(clientSocketFd) syscall failed with the following error");
        exit(EXIT_FAILURE);
    }
}