#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    const char* const testFileName = "test_file.txt";

    // Open the file for writing with the `O_APPEND` flag
    const int fd = open(testFileName, O_WRONLY | O_APPEND);
    if (fd == -1) {
        perror("An error occurred in \"open\" system call");
        printf("when trying to open the file \"%s\"\n", testFileName);
        exit(EXIT_FAILURE);
    }

    // Seek to the beginning of the file
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("An error occurred in \"lseek\" system call");
        printf("when trying to call lseek on the file \"%s\"\n", testFileName);
        exit(EXIT_FAILURE);
    }

    // Perform a write to the file
    if (write(fd, "test", 4) == -1) {
        perror("An error occurred in \"write\" system call");
        printf("when trying to call write on the file \"%s\"\n", testFileName);
        exit(EXIT_FAILURE);
    }

    // Observe that after this program runs the file contents are the same
    // as the initial data with the word "test" appended after it
}
