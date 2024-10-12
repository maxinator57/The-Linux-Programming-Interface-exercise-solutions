#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>


extern const sa_family_t SOCKET_ADDRESS_FAMILY;
extern const int SOCKET_TYPE;
enum { BUF_SIZE = 100 };


// Creates and fills the server socket address structure.
struct sockaddr_un GetServerSocketAddress();