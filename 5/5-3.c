#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if ((argc != 3 && argc != 4)
     || strcmp(argv[1], "-h") == 0
     || strcmp(argv[1], "--help") == 0
    ) {
        printf("Usage: %s FILENAME NUM_BYTES [x]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 4 && strcmp(argv[3], "x") != 0) {
        printf("Error: if the 3rd (optional) argument is present, it must have the value of \"x\"");
        exit(EXIT_FAILURE);
    }
    const int lseekBeforeEachWrite = argc == 4;

    const char* fileName = argv[1]; 
    const int oFlags = O_WRONLY | O_CREAT | (lseekBeforeEachWrite ? 0 : O_APPEND); 
    const int fd = open(fileName, oFlags, 0666);
    if (fd == -1) {
        perror("An error occurred in \"open\" system call");
        printf("when trying to open the file \"%s\"\n", fileName);
        exit(EXIT_FAILURE);
    }

    const int numBytes = atoi(argv[2]);
    if (numBytes <= 0) {
        printf("Error: the number of bytes must be a positive integer\n");
        exit(EXIT_FAILURE); 
    }

    const char buf[] = {57};
    for (int i = 0; i != numBytes; ++i) {
        if (lseekBeforeEachWrite && lseek(fd, 0, SEEK_END) == -1) {
            perror("An error occurred in \"lseek\" system call");
            printf("when trying to call lseek on the file \"%s\"\n", fileName);
            exit(EXIT_FAILURE); 
        }
        if (write(fd, buf, 1) == -1) {
            perror("An error occurred in \"write\" system call");
            printf("when trying to write to the file \"%s\"\n", fileName);
            exit(EXIT_FAILURE); 
        }
    }
}
