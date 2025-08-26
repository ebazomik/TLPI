#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char ** argv){

    int opt, fd;
    char * filepath;

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


    int flags = O_APPEND | O_RDWR;
    if((fd = open(filepath, flags)) == -1){
        printf("Error on open file: %s\n", filepath);
    }

    int newFd = fcntl(fd, F_DUPFD);

    printf("fd is %d\n", fd);
    printf("new fd is %d\n", newFd);

    close(fd);
    close(newFd);

    return 0;
}
