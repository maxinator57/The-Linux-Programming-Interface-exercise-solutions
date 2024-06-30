// #define _FILE_OFFSET_BITS 64 // on 64-bit architectures this macro is not needed
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char *argv[]) { 
    if (argc != 3
     || strcmp(argv[1], "-h") == 0
     || strcmp(argv[1], "--help") == 0
    ) {
        printf("Usage: %s PATH OFFSET\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const int fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("An error occurred in \"open\" system call");
        printf("when trying to open the file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    const off_t offset = atoll(argv[2]);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("An error occurred in \"lseek\" system call");
        printf("when trying to call lseek on the file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (write(fd, "test", 4) == -1) {
        perror("An error occurred in \"write\" system call");
        printf("when trying to write to the file \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE); 
    }
}
