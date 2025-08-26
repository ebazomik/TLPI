#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char ** argv){
    
    if(argc != 3){
        printf("Error: usage %s [oldfd] [newFd]\n", argv[0]);
        return 1;
    }

    int oldFd = atoi(argv[1]);
    int newFd = atoi(argv[2]);

    /* atoi returns 0 in case of conversion error,
    so you could check the result of this part more robustly. */

    if(oldFd == newFd){
        if(fcntl(oldFd, F_GETFL) == -1) {
            printf("errno: %d\n", errno);
            printf("errno msg: %s\n", strerror(errno));
            return 1;
        }
    }

    int fd = fcntl(oldFd, F_DUPFD, newFd);

    if(){
        printf("Error on duplicate fd\n");
        printf("errno: %d\n", errno);
        printf("errno msg: %s\n", strerror(errno));
        return 1;
    }

    printf("Old fd -> %d\n", oldFd);
    printf("New fd -> %d\n", newFd);

    close(fd);

    return 0;
}
