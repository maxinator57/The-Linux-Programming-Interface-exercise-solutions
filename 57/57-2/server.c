#include "common.h"


enum { BACKLOG = 5 };


int main(int argc, char* argv[]) {
    // Create the struct with server socket address.
    const struct sockaddr_un serverSocketAddress = GetServerSocketAddress();

    // Open the file descriptor for the server socket.
    const int serverSocketFd = socket(SOCKET_ADDRESS_FAMILY, SOCKET_TYPE, 0);
    if (serverSocketFd == -1) {
        perror("socket() system call failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Try to bind the server socket file descriptor
    // to the server socket address.
    if (bind(serverSocketFd,
             (struct sockaddr*) &serverSocketAddress,
             sizeof(serverSocketAddress)
    ) == -1) {
        perror("bind() syscall failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Start listening on the server socket.
    if (listen(serverSocketFd, BACKLOG) == -1) {
        perror("listen() syscall failed with the following error");
        exit(EXIT_SUCCESS);
    }

    // Create a buffer for storing input data from clients.
    char buf[BUF_SIZE];

    // Handle client connections iteratively.
    for (;;) {
        // Accept a connection from some client.
        const int clientPeerSocketFd = accept(serverSocketFd, NULL, NULL);
        if (clientPeerSocketFd == -1) {
            perror("accept() syscall failed with the following error");
            exit(EXIT_SUCCESS);
        }
        // Read data from the client in an infinite loop.
        for(;;) {
            // Try to read some bytes from the client.
            const ssize_t nBytesRead = read(clientPeerSocketFd, buf, BUF_SIZE);
            if (nBytesRead == 0) { // the client has finished sending the data
                break;
            } else if (nBytesRead == -1) {
                perror(
                    "read(clientPeerSocketFd) syscall failed with the following error"
                );
                exit(EXIT_FAILURE);
            }
            // Try to write all the data received from the client to stdout.
            if (write(STDOUT_FILENO, buf, nBytesRead) != nBytesRead) {
                printf("Partial/failed write to stdout");
                exit(EXIT_FAILURE);
            }
        }
        // Try to close the client peer socket file descriptor.
        if (close(clientPeerSocketFd) == -1) perror(
            "close(clientPeerSocketFd) syscall "
            "failed with the following error"
        );
    } 
}

