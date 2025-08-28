/* Write a program to verify that duplicated file descriptors share a file offset value and open file status flag */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char ** argv){

    if(argc != 2){
        printf("Usage: pregram [file]\n");
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if(fd == -1){
        printf("Error when try to open %s\n", argv[1]);
        return 1;
    }

    int fd2 = dup(fd);
    if(fd2 == -1){
        printf("Error when try duplicate fd %d\n", fd);
        return 1;
    }

    int off1 = lseek(fd, 0, SEEK_SET);
    int off2 = lseek(fd2, 0, SEEK_SET);

    if(off1 == -1 || off2 == -1){
        printf("fd: %d -> off: %d\n", fd, off1);
        printf("fd: %d -> off: %d\n", fd2, off2);
        return 1;
    }

    printf("Offset of fd %d is: %d\n", fd, off1);
    printf("Offset of fd %d is: %d\n", fd2, off2);

    int flags = fcntl(fd, F_GETFD);
    if(flags == -1){
        printf("Can't recover flags of fd: %d\n", fd);
        return 1;
    }

    printf("FLags of fd %d is %d\n", fd, flags);

    close(fd);
    close(fd2);

    return 0;
}
