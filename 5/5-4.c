#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int Dup(int fd) {
    return fcntl(fd, F_DUPFD, 0);
}

int IsOpenFileDescriptor(int fd) {
    const int statusFlags = fcntl(fd, F_GETFL);
    return statusFlags != -1;
}

int Dup2(int oldFd, int newFd) {
    if (newFd == oldFd) {
        if (IsOpenFileDescriptor(oldFd)) {
            return oldFd; 
        } else {
            // `errno` is already set appropriately
            // by `IsOpenFileDescriptor()`
            return -1;
        }
    } else if (IsOpenFileDescriptor(newFd)) {
        if (close(newFd) == -1) return -1;
        else return fcntl(oldFd, F_DUPFD, newFd);
    } else {
        return fcntl(oldFd, F_DUPFD, newFd);
    }
}


int main() {
    const int fd = Dup2(STDOUT_FILENO, 57);
    if (fd == -1) {
        perror("An error occurred in \"Dup2\" call");
        exit(EXIT_FAILURE);
    }
    assert(fd == 57);
    if (write(fd, "abacaba\n", 8) == -1) {
        perror("An error occurred in \"write\" system call");
        exit(EXIT_FAILURE);
    }

    // We can experiment with the `oldFd` and `newFd` passed to `Dup2`
    if (Dup2(10000000, 10000000) == -1) {
        perror("As expected");
        exit(EXIT_FAILURE);
    }
}
