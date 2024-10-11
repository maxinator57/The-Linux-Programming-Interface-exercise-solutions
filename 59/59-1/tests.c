#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rlbuf.h"


void TestSmallBufRegularFile() {
    const int fd = open("./test_data/test_regular_file.txt", O_RDONLY);
    struct TReadlineBuf rlbuf;
    // Choose a small size for the readline buffer: only 2 bytes
    const size_t rlbufSize = 2;
    if (ReadLineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("TestSmallBufRegularFile failed with the following error:");
        exit(EXIT_FAILURE);
    }
    char line[10];
    {
        // The first line in the file is `"Hello,\n"`
        const char* expectedLine = "Hello,\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The second line in the file is `"world\n"`
        const char* expectedLine = "world\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The third line in the file is `"!\n"`
        const char* expectedLine = "!\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The fourth line in the file is `"Hello, world!\n"`
        const char* expectedLine = "Hello, world!\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        // Because the length of the `line` buffer is only 10,
        // the 5 bytes "rld!\n" were truncated, which is reflected
        // in the value of `nBytesRead` (which is 15, not 10)
        assert(strncmp(line, "Hello, wor", sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file
        // Therefore, all subsequent reads return 0
        for (int _ = 0; _ != 5; ++_) { // here we perform 5 reads just to be sure
            const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }
    ReadlineBufFree(&rlbuf);
    puts("TestSmallBufRegularFile OK");
}


void TestBigBufRegularFile() {
    const int fd = open("./test_data/test_regular_file.txt", O_RDONLY);
    struct TReadlineBuf rlbuf;
    // Choose a big size for the readline buffer: 128 bytes
    const size_t rlbufSize = 128;
    if (ReadLineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("TestSmallBufRegularFile failed with the following error:");
        exit(EXIT_FAILURE);
    }
    char line[10];
    {
        // The first line in the file is `"Hello,\n"`
        const char* expectedLine = "Hello,\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The second line in the file is `"world\n"`
        const char* expectedLine = "world\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The third line in the file is `"!\n"`
        const char* expectedLine = "!\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        line[nBytesRead] = 0;
        assert(strcmp(line, expectedLine) == 0);
    }
    {
        // The fourth line in the file is `"Hello, world!\n"`
        const char* expectedLine = "Hello, world!\n";
        const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        // Because the length of the `line` buffer is only 10,
        // the 5 bytes "rld!\n" were truncated, which is reflected
        // in the value of `nBytesRead` (which is 15, not 10)
        assert(strncmp(line, "Hello, wor", sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file
        // Therefore, all subsequent reads return 0
        for (int _ = 0; _ != 5; ++_) { // here we perform 5 reads just to be sure
            const ssize_t nBytesRead = ReadLineBuf(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }
    ReadlineBufFree(&rlbuf);
    puts("TestBigBufRegularFile OK");
}


int main() {
    TestSmallBufRegularFile();
    TestBigBufRegularFile();
}
