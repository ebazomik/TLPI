#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv){

    int opt;
    char *filePath = NULL;
    int numBytes = 0;
    int flagX = 0;

    if(argc < 3){
        printf("Usage: %s -f [filename] -b [num-bytes] -x [x]\n", argv[0]);
        return 1;
    }

    while((opt = getopt(argc, argv, "f:b:x")) != -1){
        switch(opt){
            case 'f':
                filePath = optarg;
                break;
            case 'b':
                numBytes = atoi(optarg);
                break;
            case 'x':
                flagX = 1;
                break;
            default:
                printf("Usage: %s -f [filename] -b [num-bytes] -x [x]\n", argv[0]);
                return 1;
        }
    }
    

    int fd;
    int flags = O_WRONLY | O_CREAT;
    if(!flagX) flags |= O_APPEND;
    mode_t mode = S_IWUSR | S_IRUSR;
    fd = open(filePath, flags, mode);
    if(fd == -1){
        printf("Error while open  %s\n", argv[2]);
        return 1;
    }

    char byte = flagX ? 'A' : 'B';
    for(int i = 0; i < numBytes; i++){
        if (flagX) lseek(fd, 0, SEEK_END);
        if(write(fd, &byte, 1) == -1){
            printf("Error on write byte\n");
            break;
        }
    }

    close(fd);

    return 0;
}
