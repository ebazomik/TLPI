#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
    char buffer[BUFF_SIZE + 1];
    int file_source, file_target, flags, mode, bytes_writted, bytes_readed;
    
    if(argc < 3){
        printf("%s [file-source] [file-target]\n", argv[0]);
        printf("%s\n", strerror(errno));
        return 1;
    }
    
    file_source = open(argv[1], O_RDONLY);
    if(file_source == -1){
        printf("Error on open %s\n", argv[1]);
        printf("%s\n", strerror(errno));
        return 1;
    }

    flags = O_RDWR | O_CREAT;
    mode = S_IRUSR | S_IWUSR;
    
    file_target = open(argv[2], flags, mode);
    if(file_target == -1){
        printf("Error on open %s\n", argv[2]);
        printf("%s", strerror(errno));
        close(file_source);
        return 1;
    }

    while( (bytes_readed = read(file_source, buffer, BUFF_SIZE)) > 0 ){
        buffer[bytes_readed] = '\0';
        bytes_writted = write(file_target, buffer, bytes_readed);
        if(bytes_writted != bytes_readed){
            printf("Error on write or partial write occurred\n");
            printf("%s\n", strerror(errno));
            close(file_source);
            close(file_target);
            return 1;
        }
    }
    
    int close_source = close(file_source);
    int close_target = close(file_target);
    if(close_source != 0 || close_target != 0){
        printf("Error on close target or source file\n");
        printf("%s\n", strerror(errno));
    }

    return 0;
}
