#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rlbuf.h"


void TestSmallRlBufRegularFile() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // The buffer that is passed into each `ReadlineBuf()`
    // call as a storage for the newly read line.
    char line[10];

    // Read the lines from the file one by one.
    {
        // The first line in the file is `"Hello,\n"`.
        const char* expectedLine = "Hello,\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The second line in the file is `"world\n"`.
        const char* expectedLine = "world\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The third line in the file is `"!\n"`.
        const char* expectedLine = "!\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The fourth line in the file is `"Hello, world!\n"`.
        const char* expectedLine = "Hello, world!\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        // Because the length of the `line` buffer is only 10,
        // the 5 bytes "rld!\n" were truncated, which is reflected
        // in the value of `nBytesRead` (which is 15, not 10).
        assert(strncmp(line, "Hello, wor", sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file.
        // Therefore, all subsequent reads return 0.
        for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }

    // Free the `TReadlineBuf` struct.
    ReadlineBufFree(&rlbuf);

    // Try to close the file descriptor for the file with test data.
    if (close(fd) == -1) {
        perror("close(fd) failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestSmallRlBufRegularFile OK");
}


void TestBigRlBufRegularFile() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a big size for `rlbuf` internal buffer:
    // 128 bytes, so that the whole file contents fit into this buffer.
    const size_t rlbufSize = 128;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // The buffer that is passed into each `ReadlineBuf()`
    // call as a storage for the newly read line.
    char line[10];

    // Read the lines from the file one by one.
    // The results should be exactly the same as in the
    // `TestSmalRlBufRegularFile` test.
    {
        // The first line in the file is `"Hello,\n"`.
        const char* expectedLine = "Hello,\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The second line in the file is `"world\n"`.
        const char* expectedLine = "world\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The third line in the file is `"!\n"`.
        const char* expectedLine = "!\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The fourth line in the file is `"Hello, world!\n"`.
        const char* expectedLine = "Hello, world!\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        // Because the length of the `line` buffer is only 10,
        // the 5 bytes "rld!\n" were truncated, which is reflected
        // in the value of `nBytesRead` (which is 15, not 10).
        assert(strncmp(line, "Hello, wor", sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file.
        // Therefore, all subsequent reads return 0.
        for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }

    // Free the `TReadlineBuf` struct.
    ReadlineBufFree(&rlbuf);

    // Try to close the file descriptor for the file with test data.
    if (close(fd) == -1) {
        perror("close(fd) failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestBigRlBufRegularFile OK");
}


void TestSmallRlBufRegularFileLongLines() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file_long_lines.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // The buffer that is passed into each `ReadlineBuf()`
    // call as a storage for the newly read line.
    // Most of the lines in the test file are longer than
    // the size of this buffer.
    char line[10]; 

    // Read the lines from the file one by one.
    {
        // The first line in the file is `"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"`.
        const char* expectedLine = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The second line in the file is `"bbbbbbbbbbbbbbbbbbb\n"`.
        const char* expectedLine = "bbbbbbbbbbbbbbbbbbb\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The third line in the file is `"ccccccccccccccccccccccc\n"`.
        const char* expectedLine = "ccccccccccccccccccccccc\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The fourth line in the file is `"ddddd\n"`.
        const char* expectedLine = "ddddd\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The fifth line in the file is `"eeeeeeeeeeeeeeeeeeeeeeeeeee\n"`.
        const char* expectedLine = "eeeeeeeeeeeeeeeeeeeeeeeeeee\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The sixth line in the file is `"ffffffffffffffff\n"`.
        const char* expectedLine = "ffffffffffffffff\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The seventh line in the file is `"gggg\n"`.
        const char* expectedLine = "gggg\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The eighth line in the file is `"hhhhhhhhhhhhhhhhhhhhhhhhhh\n"`.
        const char* expectedLine = "hhhhhhhhhhhhhhhhhhhhhhhhhh\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file.
        // Therefore, all subsequent reads return 0.
        for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }

    // Free the `TReadlineBuf` struct.
    ReadlineBufFree(&rlbuf);

    // Try to close the file descriptor for the file with test data.
    if (close(fd) == -1) {
        perror("close(fd) failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestSmallRlBufRegularFileLongLines OK");
}


void TestBigRlBufRegularFileLongLines() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file_long_lines.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a big size for `rlbuf` internal buffer:
    // 128 bytes, so that the whole file contents fit into this buffer.
    const size_t rlbufSize = 128;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // The buffer that is passed into each `ReadlineBuf()`
    // call as a storage for the newly read line.
    // Most of the lines in the test file are longer than
    // the size of this buffer.
    char line[10]; 

    // Read the lines from the file one by one.
    // The results should be exactly the same as in the
    // `TestBigRlBufRegularFileLongLines` test.
    {
        // The first line in the file is `"aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"`.
        const char* expectedLine = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The second line in the file is `"bbbbbbbbbbbbbbbbbbb\n"`.
        const char* expectedLine = "bbbbbbbbbbbbbbbbbbb\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The third line in the file is `"ccccccccccccccccccccccc\n"`.
        const char* expectedLine = "ccccccccccccccccccccccc\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The fourth line in the file is `"ddddd\n"`.
        const char* expectedLine = "ddddd\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The fifth line in the file is `"eeeeeeeeeeeeeeeeeeeeeeeeeee\n"`.
        const char* expectedLine = "eeeeeeeeeeeeeeeeeeeeeeeeeee\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The sixth line in the file is `"ffffffffffffffff\n"`.
        const char* expectedLine = "ffffffffffffffff\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // The seventh line in the file is `"gggg\n"`.
        const char* expectedLine = "gggg\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The eighth line in the file is `"hhhhhhhhhhhhhhhhhhhhhhhhhh\n"`.
        const char* expectedLine = "hhhhhhhhhhhhhhhhhhhhhhhhhh\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, sizeof(line)) == 0);
    }
    {
        // There are no more lines left in the file.
        // Therefore, all subsequent reads return 0.
        for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }

    // Free the `TReadlineBuf` struct.
    ReadlineBufFree(&rlbuf);

    // Try to close the file descriptor for the file with test data.
    if (close(fd) == -1) {
        perror("close(fd) failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestBigRlBufRegularFileLongLines OK");
}


void TestSmallRlBufRegularFileNoNewlineAtEnd() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file_no_newline_at_end.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // The buffer that is passed into each `ReadlineBuf()`
    // call as a storage for the newly read line.
    char line[10];

    // Read the lines from the file one by one.
    {
        // The first line in the file is `"Hello,\n"`.
        const char* expectedLine = "Hello,\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The second line in the file is `"world\n"`.
        const char* expectedLine = "world\n";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // The third (and last) line in the file is `"!"`,
        // without the `'\n'` at the end.
        const char* expectedLine = "!";
        const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
        assert(nBytesRead == strlen(expectedLine));
        assert(strncmp(line, expectedLine, nBytesRead) == 0);
    }
    {
        // There are no more lines left in the file.
        // Therefore, all subsequent reads return 0.
        for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == 0);
        }
    }

    // Free the `TReadlineBuf` struct.
    ReadlineBufFree(&rlbuf);

    // Try to close the file descriptor for the file with test data.
    if (close(fd) == -1) {
        perror("close(fd) failed with the following error:");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestSmallRlBufRegularFileNoNewlineAtEnd OK");
}


int main() {
    TestSmallRlBufRegularFile();
    TestBigRlBufRegularFile();
    TestSmallRlBufRegularFileLongLines();
    TestBigRlBufRegularFileLongLines();
    TestSmallRlBufRegularFileNoNewlineAtEnd();
}
