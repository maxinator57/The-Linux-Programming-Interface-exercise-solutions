#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


enum { BUF_SIZE = 1024 };
char BUF[BUF_SIZE];


size_t Read() {
    const ssize_t nBytesRead = read(STDIN_FILENO, BUF, BUF_SIZE);
    if (nBytesRead == -1) {
        perror("An error occured in \"read\" system call");
        exit(EXIT_FAILURE);
    }
    return nBytesRead;
}


void Write(int fd, size_t nBytesToWrite) {
    size_t totalNBytesWritten = 0;
    while (totalNBytesWritten != nBytesToWrite) {
        ssize_t nBytesWritten = write(fd, BUF, nBytesToWrite - totalNBytesWritten);
        if (nBytesWritten == -1) {
            perror("An error occured in \"write\" system call");
            exit(EXIT_FAILURE);
        }
        totalNBytesWritten += nBytesWritten;
    }
}


int main(int argc, char* const argv[]) {
    const char* usage = "Usage: %s [-a] [FILE...]\n";
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
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
        int fd = open(argv[optind + i], openFlags, 0664);
        if (fd == -1) {
            perror("An error occurred in \"open\" system call");
            exit(EXIT_FAILURE);
        } else {
            outputFileFds[i + 1] = fd;
        }
    }

    size_t nBytesRead;
    while ((nBytesRead = Read()) != 0) {
        for (int i = 0; i != nOutputFiles + 1; ++i) {
            Write(outputFileFds[i], nBytesRead);
        }
    }
}
