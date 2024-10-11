#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rlbuf.h"


static ssize_t ReadRobust(const int fd, void* buf, const size_t nBytesToRead) {
    for(;;) {
        const ssize_t nBytesRead = read(fd, buf, nBytesToRead);
        // If the read() call was interrupted by a signal, then retry
        if (nBytesRead == -1 && errno == EINTR) continue;
        else return nBytesRead;
    }
}

int ReadLineBufInit(int fd, struct TReadlineBuf* rlbuf, const size_t rlbufSize) {
    if (rlbufSize == 0) {
        errno = EINVAL;
        return -1;
    } else if ((rlbuf->Buf = (char*) malloc(rlbufSize)) == NULL) {
        // Error on `malloc()` call
        return -1;
    }
    rlbuf->BufSize = rlbufSize;
    rlbuf->ActualBufSize = 0;
    rlbuf->FirstUnreadCharacterIdx = 0;
    rlbuf->Fd = fd;
    rlbuf->FinishedReading = 0;
    return 0;
}

void ReadlineBufFree(struct TReadlineBuf* rlbuf) {
    free(rlbuf->Buf);
    rlbuf->BufSize = 0;
    rlbuf->ActualBufSize = 0;
    rlbuf->FirstUnreadCharacterIdx = 0;
    // Setting `rlbuf->Fd` to `0` may have slightly weird
    // meaning considering that `0` is usually the file
    // descriptor corres ponding to stdin of the program
    //
    // Setting `FinishedReading` to 1 would be semantically
    // incorrect, because, due to the semantics of the field
    // `rlbuf->FinishedReading`, its value is equal to 1 if
    // and only if we have already obtained EOF when reading
    // from `rlbuf->Fd`
}

static int ReadlineBufUpdate(struct TReadlineBuf* rlbuf) {
    if (rlbuf->FinishedReading || rlbuf->FirstUnreadCharacterIdx != rlbuf->ActualBufSize) {
        errno = EINVAL;
        return -1;
    }
    const ssize_t nBytesRead = ReadRobust(rlbuf->Fd, rlbuf->Buf, rlbuf->BufSize);
    if (nBytesRead == -1) {
        // Some error occurred while reading
        rlbuf->FinishedReading = 1;
        return -1;
    } else {
        if (nBytesRead == 0) {
            // Got EOF when trying to read from the file
            rlbuf->FinishedReading = 1;
        }
        rlbuf->ActualBufSize = nBytesRead;
        rlbuf->FirstUnreadCharacterIdx = 0;
        return 0;
    }
}

static int IsValid(struct TReadlineBuf* rlbuf) {
    return rlbuf != NULL
        && rlbuf->Buf != NULL
        && rlbuf->BufSize != 0
        && rlbuf->ActualBufSize <= rlbuf->BufSize
        && rlbuf->FirstUnreadCharacterIdx <= rlbuf->ActualBufSize;
}

static size_t GetNumBytesAvailableForReading(
    const struct TReadlineBuf* rlbuf
) {
    return rlbuf->ActualBufSize - rlbuf->FirstUnreadCharacterIdx;
}

ssize_t ReadLineBuf(struct TReadlineBuf* rlbuf, char* dst, const size_t n) {
    // Standard argument validity check, basically
    // copy-pasted from listing 59-1 from the book
    if (n <= 0 || dst == NULL || !IsValid(rlbuf)) {
        errno = EINVAL;
        return -1;
    }
    // If we have already finished reading from `rlbuf`,
    // just return `0`
    if (rlbuf->FinishedReading) {
        return 0;
    }
    // Loop invariant: if there are unread bytes in the file
    // then there are at least some unread bytes in `rlbuf->Buf`
    //
    // If there are currently no bytes available for reading in `rlbuf->Buf`,
    // try to uphold the loop invariant by updating `rlbuf`
    if (
        GetNumBytesAvailableForReading(rlbuf) == 0 &&
        ReadlineBufUpdate(rlbuf) == -1 // some error occured when reading from `rlbuf`
    ) return -1;
    // Start the loop; if we have reached this point,
    // the loop invariant is guaranteed to be holding
    size_t nBytesRead = 0, nBytesCopiedToDst = 0;
    for (
        size_t nBytesAvailableForReading = GetNumBytesAvailableForReading(rlbuf);
        nBytesAvailableForReading != 0;
        nBytesAvailableForReading = GetNumBytesAvailableForReading(rlbuf)
    ) {
        // Try to find the newline character `'\n'` among the
        // next `nBytesAvailableForReading` bytes in `rlbuf->Buf`
        // The start of the part of `rlbuf->Buf` with unread bytes
        const char* const fromStart = rlbuf->Buf + rlbuf->FirstUnreadCharacterIdx;
        // Try to find an occurrence of `'\n'` in this part of `rlbuf->Buf` 
        const char* const fromEnd = (char*) memchr(fromStart, '\n', nBytesAvailableForReading);
        // If an occurrence of `'\n'` was not found, we should
        // copy `nBytesAvailableForReading` bytes into `dst`
        // Otherwise, we should copy all bytes up to the first occurrence of `'\n'`
        const size_t nBytesToCopy = fromEnd == NULL
                                  ? nBytesAvailableForReading
                                  : fromEnd - fromStart + 1;
        // However, we can't copy more bytes to `dst` than the amount of
        // the remaining space in `dst` buffer
        const size_t remainingDstSpace = n - nBytesCopiedToDst;
        // If this remaining space is positive, we can copy some
        // bytes to `dst`
        if (remainingDstSpace > 0) {
            const size_t nBytesToActuallyCopy = nBytesToCopy < remainingDstSpace
                                              ? nBytesToCopy
                                              : remainingDstSpace;
            memcpy(dst + nBytesCopiedToDst, fromStart, nBytesToActuallyCopy);
            // Update the `nBytesCopiedToDst` counter
            nBytesCopiedToDst += nBytesToActuallyCopy;
        }
        // Update the `nBytesRead` counter
        nBytesRead += nBytesToCopy;
        // Update the `rlbuf` invariant: `FirstUnreadCharacterIdx` is
        // the index of the first unread byte
        rlbuf->FirstUnreadCharacterIdx += nBytesToCopy;
        // If the `'\n'` was actually found, stop the loop, because we have
        // successfully read the next line
        if (fromEnd != NULL) break;
        // If there are currently no bytes available for reading,
        // try to uphold the loop invariant by updating `rlbuf`
        // (i.e. reading some more bytes from the file into `rlbuf`)
        if (
            GetNumBytesAvailableForReading(rlbuf) == 0 &&
            !rlbuf->FinishedReading &&
            ReadlineBufUpdate(rlbuf) == -1 // some error occurred when updating `rlbuf`
        ) return -1;
    }
    return nBytesRead;
}
