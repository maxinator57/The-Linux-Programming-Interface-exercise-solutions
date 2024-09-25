#pragma once

#include <sys/un.h>
#include <sys/socket.h>


enum {DATAGRAM_MAX_SIZE = 16};
static const char* SERVER_SOCKET_PATH = "/tmp/ex_57-1_socket";
