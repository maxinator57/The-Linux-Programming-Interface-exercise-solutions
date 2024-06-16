#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    size_t totalNBytesWritten = 0;
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


int main(int argc, char* const argv[]) {
    const char* usage = "Usage: %s [-a] [FILE]...\n";
    if (argc >= 2 && (
        strcmp(argv[1], "-h") == 0
     || strcmp(argv[1], "--help") == 0
    )) {
        printf(usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    int append = 0; 
    int opt;
    while ((opt = getopt(argc, argv, "a")) != -1) {
        if (opt == 'a') append = 1;
        else {
            printf(usage, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    const int openFlags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);

    const int nOutputFiles = argc - optind;
    int outputFileFds[nOutputFiles + 1]; outputFileFds[0] = STDOUT_FILENO;
    for (int i = 0; i != nOutputFiles; ++i) {
        const char* curFileName = argv[optind + i];
        const int fd = open(curFileName, openFlags, 0664);
        if (fd == -1) {
            perror("An error occurred in \"open\" system call");
            printf("when trying to open the file \"%s\"\n", curFileName);
            exit(EXIT_FAILURE);
        } else {
            outputFileFds[i + 1] = fd;
        }
    }

    size_t nBytesRead;
    while ((nBytesRead = Read(STDIN_FILENO)) != 0) {
        for (int i = 0; i != nOutputFiles + 1; ++i) {
            Write(outputFileFds[i], nBytesRead);
        }
    }
}
