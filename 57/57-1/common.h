#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>



enum {DATAGRAM_MAX_SIZE = 16};
static const char* SERVER_SOCKET_PATH = "/tmp/ex_57-1_socket";
