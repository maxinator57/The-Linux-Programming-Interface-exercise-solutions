#include <assert.h>
#include <bits/types/struct_timeval.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include "rlbuf.h"


void TestSmallRlBufRegularFile() {
    // Try to open the file with test data.
    const int fd = open("./test_data/test_regular_file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open() failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error");
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
        perror("close(fd) failed with the following error");
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
        perror("open() failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a big size for `rlbuf` internal buffer:
    // 128 bytes, so that the whole file contents fit into this buffer.
    const size_t rlbufSize = 128;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error");
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
        perror("close(fd) failed with the following error");
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
        perror("open() failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error");
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
        perror("close(fd) failed with the following error");
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
        perror("open() failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a big size for `rlbuf` internal buffer:
    // 128 bytes, so that the whole file contents fit into this buffer.
    const size_t rlbufSize = 128;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error");
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
        perror("close(fd) failed with the following error");
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
        perror("open() failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Create and initialize the `TReadlineBuf` struct.
    struct TReadlineBuf rlbuf;
    // In this test we choose a very small size
    // for `rlbuf` internal buffer: only 2 bytes.
    const size_t rlbufSize = 2;
    if (ReadlineBufInit(fd, &rlbuf,  rlbufSize) == -1) {
        perror("ReadlineBufInit failed with the following error");
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
        perror("close(fd) failed with the following error");
        exit(EXIT_FAILURE);
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestSmallRlBufRegularFileNoNewlineAtEnd OK");
}


static void Wait(const pid_t childPid) {
    // Wait for the child process.
    if (waitpid(childPid, NULL, 0) == -1) {
        perror("waitpid() failed with the following error");
        exit(EXIT_FAILURE);
    }
}


void TestPipeFile() {
    int pipeFds[2]; if (pipe(pipeFds) == -1) {
        perror("pipe() failed with the following error");
        exit(EXIT_FAILURE);
    }
    int pid; if ((pid = fork()) == -1) {
        perror("fork() failed with the following error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
        // Close the reading end of the pipe.
        if (close(pipeFds[0]) == -1) {
            perror("close(pipeFds[0]) in child failed with the following error");
            exit(EXIT_FAILURE);
        }
        // The child writes a line to the pipe every second.
        const char* lines[] = {"Hello,\n", "world\n", "!\n", "Hello, world!\n"};
        const size_t numLines = sizeof(lines) / sizeof(*lines);
        for (size_t i = 0; i != numLines; ++i) {
            // Sleep for 1 second before sending the current line
            sleep(1);
            const char* curLine = lines[i];
            const size_t nBytesToWrite = strlen(curLine);
            const int nBytesWritten = write(pipeFds[1], curLine, nBytesToWrite);
            if (nBytesWritten == -1) {
                perror("write() to pipe failed with the followring error");
                exit(EXIT_FAILURE);
            } else if (nBytesWritten < nBytesToWrite) {
                perror("partial write()");
                exit(EXIT_FAILURE);
            }
        }
        // Close the writing end of the pipe.
        if (close(pipeFds[1]) == -1) {
            perror("close(pipeFds[1]) in child failed with the following error");
            exit(EXIT_FAILURE);
        }
        // We actually have to terminate the child with exit()
        // because otherwise the child would go on runninng the
        // subsequent tests after returning from this test.
        exit(EXIT_SUCCESS);
    } else { // parent process
        // Close the writing end of the pipe
        if (close(pipeFds[1]) == -1) {
            perror("close(pipeFds[1]) in parent failed with the following error");
            Wait(pid);
            exit(EXIT_FAILURE);
        }
        // Create and initialize the `TReadlineBuf` struct.
        struct TReadlineBuf rlbuf;
        // In this test we deliberately choose a big
        // size for `rlbuf` internal buffer: 1Kb.
        const size_t rlbufSize = 1024; // 1Kb expressed in bytes
        if (ReadlineBufInit(pipeFds[0], &rlbuf,  rlbufSize) == -1) {
            perror("ReadlineBufInit() failed with the following error");
            Wait(pid);
            exit(EXIT_FAILURE);
        }

        // The buffer that is passed into each `ReadlineBuf()`
        // call as a storage for the newly read line.
        char line[10];

        // Read the lines from the pipe one by one.
        // Every read has to take at most 1 second + eps.
        // struct timeval curTime; if (gettimeofday(&curTime, NULL) == -1) {
        //     perror("gettimeofday() failed with the following error");
        //     Wait(pid);
        //     exit(EXIT_FAILURE);
        // }
        {
            // The first line in the pipe is `"Hello,\n"`.
            const char* expectedLine = "Hello,\n";
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == strlen(expectedLine));
            assert(strncmp(line, expectedLine, nBytesRead) == 0);
        }
        {
            // The second line in the pipe is `"world\n"`.
            const char* expectedLine = "world\n";
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == strlen(expectedLine));
            assert(strncmp(line, expectedLine, nBytesRead) == 0);
        }
        {
            // The third line in the pipe is `"!\n"`.
            const char* expectedLine = "!\n";
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == strlen(expectedLine));
            assert(strncmp(line, expectedLine, nBytesRead) == 0);
        }
        {
            // The fourth line in the pipe is `"Hello, world!\n"`.
            const char* expectedLine = "Hello, world!\n";
            const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
            assert(nBytesRead == strlen(expectedLine));
            // Because the length of the `line` buffer is only 10,
            // the 5 bytes "rld!\n" were truncated, which is reflected
            // in the value of `nBytesRead` (which is 15, not 10).
            assert(strncmp(line, "Hello, wor", sizeof(line)) == 0);
        }
        {
            // There are no more lines left in the pipe.
            // Therefore, all subsequent reads return 0.
            for (int _ = 0; _ != 5; ++_) { // perform 5 reads just to be sure
                const ssize_t nBytesRead = Readline(&rlbuf, line, sizeof(line));
                assert(nBytesRead == 0);
            }
        }

        // Wait for the child process.
        Wait(pid);

        // Free the `TReadlineBuf` struct.
        ReadlineBufFree(&rlbuf);

        // Close the reading end of the pipe.
        if (close(pipeFds[0]) == -1) {
            perror("close(pipeFds[0]) in parent failed with the following error");
            exit(EXIT_FAILURE);
        }
    }

    // Output the message indicating that the `TReadlineBuf`
    // implementation has passed the test.
    puts("TestPipeFile OK");
}


int main() {
    TestSmallRlBufRegularFile();
    TestBigRlBufRegularFile();
    TestSmallRlBufRegularFileLongLines();
    TestBigRlBufRegularFileLongLines();
    TestSmallRlBufRegularFileNoNewlineAtEnd();
    TestPipeFile();
}
