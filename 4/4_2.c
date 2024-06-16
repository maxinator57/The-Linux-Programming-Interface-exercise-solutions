#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


enum { BUF_SIZE = 1024 };
char BUF[BUF_SIZE];


size_t Read(int fd) {
    const ssize_t nBytesRead = read(fd, BUF, BUF_SIZE);
    if (nBytesRead == -1) {
        perror("An error occurred in \"read\" system call");
        exit(EXIT_FAILURE);
    }
    return nBytesRead;
}


void Write(int fd, size_t nBytesToWrite) {
    size_t nZeroBytes = 0;
    while (nZeroBytes != nBytesToWrite && BUF[nZeroBytes] == 0) ++nZeroBytes;
    if (lseek(fd, nZeroBytes, SEEK_CUR) == -1) {
        perror("An error occurred in \"lseek\" system call");
        exit(EXIT_FAILURE);
    }
    size_t totalNBytesWritten = nZeroBytes;
    while (totalNBytesWritten != nBytesToWrite) {
        ssize_t nBytesWritten = write(
            fd,
            BUF + totalNBytesWritten,
            nBytesToWrite - totalNBytesWritten
        );
        if (nBytesWritten == -1) {
            perror("An error occurred in \"write\" system call");
            exit(EXIT_FAILURE);
        }
        totalNBytesWritten += nBytesWritten;
    }
}


int main(int argc, const char* argv[]) {
    if (argc != 3
     || strcmp(argv[1], "-h") == 0
     || strcmp(argv[1], "--help") == 0
    ) {
        printf("Usage: %s SRC DST\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int srcFd = open(argv[1], O_RDONLY);
    if (srcFd == -1) {
        perror("An error occurred in \"open\" system call");
        printf("when trying to open the file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Get permissions for the source file
    struct stat srcStatBuf;
    if (fstat(srcFd, &srcStatBuf) == -1) {
        perror("An error occurred in \"fstat\" system call");
        printf("when trying to get the info about the file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    const mode_t srcPermissions = srcStatBuf.st_mode;

    const int dstFd = open(
        argv[2],
        O_WRONLY | O_CREAT | O_TRUNC,
        // Set the same permissions for the destination file as for the source file
        srcPermissions
    );
    if (dstFd == -1) {
        perror("An error occurred in \"open\" system call");
        printf("when trying to open the file \"%s\"\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    size_t nBytesRead;
    while ((nBytesRead = Read(srcFd)) != 0) {
        Write(dstFd, nBytesRead);
    }
}