#pragma once


#include <sys/types.h>


// The struct representing a memory buffer
// for reading lines from a file
struct TReadlineBuf {
    // The buffer containing the characters
    // read from the file in advance
    char* Buf;
    // The size of the buffer
    size_t BufSize;
    // The actual size of the buffer:
    // cab be less than `BufSize` if the last call
    // to `ReadRobust()` returned less than `BufSize`
    size_t ActualBufSize;
    // The index of the first unread character;
    size_t FirstUnreadCharacterIdx;
    // The file descriptor referring to the file
    // from which the characters are read
    int Fd;
    // A bool flag indicating whether we have already got
    // an `EOF` when reading from `Fd`
    int FinishedReading;
};

// On success, allocates `rlbufSize` bytes of memory on the heap and returns `0`
// On error, performs no heap memory allocation and returns `-1`, setting `errno`
// appropriately
int ReadLineBufInit(const int fd, struct TReadlineBuf* rlbuf, const size_t rlbufSize);

// Frees the memory on the heap and resets the appropriate fields of `rlbuf`
// Always returns successfully
void ReadlineBufFree(struct TReadlineBuf* rlbuf);

// Reads the next line (but up to `n` bytes) from
// `rlbuf` into the buffer pointed to by `dst`
// If the line ends with a `'\n'` character and
// has length <= `n` bytes, then the `'\n'`
// character is also copied into `dst`
// On success, returns the number of the bytes read into `dst`
// On error, returns `-1` and sets `errno` appropriately
ssize_t ReadLineBuf(struct TReadlineBuf* rlbuf, char* dst, const size_t n);
