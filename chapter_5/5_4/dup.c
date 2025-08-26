#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char ** argv){

    int opt, fd;
    char * filepath = NULL;

    if(argc < 2){
        printf("Usage: -f [filePath]\n");
        return 1;
    }

    while((opt = getopt(argc, argv, "f:")) != -1){
        if(opt == 'f'){
            filepath = optarg;
        } else {
            printf("Usage: -f [filePath]\n");
            return 1;
        }
    }

    if(filepath == NULL){
        printf("Error: usage with -f [pathname] arg\n");
        return 1;
    }


    int flags = O_APPEND | O_RDWR;
    if((fd = open(filepath, flags)) == -1){
        printf("Error on open file: %s\n", filepath);
        return 1;
    }

    int newFd = fcntl(fd, F_DUPFD);
    if(newFd == -1){
        printf("Error on duplication fd %d\n", fd);
        close(fd);
        return 1;
    }

    printf("fd is %d\n", fd);
    printf("new fd is %d\n", newFd);

    close(fd);
    close(newFd);

    return 0;
}
