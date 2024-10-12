#include <string.h>

#include "common.h"


static const char* SERVER_SOCKET_NAME = "abstract_server_socket";
const sa_family_t SOCKET_ADDRESS_FAMILY = AF_UNIX;
const int SOCKET_TYPE = SOCK_STREAM; 


struct sockaddr_un GetServerSocketAddress() {
    struct sockaddr_un serverSocketAddress;
    memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
    serverSocketAddress.sun_family = SOCKET_ADDRESS_FAMILY;
    strncpy(
        // The 0-th byte of `serverSocketAddress.sun_path` array is `0`,
        // hence, on binding, the socket will be created not in the
        // file system, but in the linux abstract socket namespace.
        /* dst */ &serverSocketAddress.sun_path[1],
        /* src */ SERVER_SOCKET_NAME,
        /* len */ sizeof(serverSocketAddress.sun_path) - 2
    );
    return serverSocketAddress;
}