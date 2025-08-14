#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFF_SIZE 4096

int main(int argc, char *argv[]){
    char buffer[BUFF_SIZE];
    int file_source, file_target;
    ssize_t bytes_readed;

    if(argc != 3){
        printf("%s [file-source] [file-target]\n", argv[0]);
        return 1;
    }
    
    file_source = open(argv[1], O_RDONLY);
    if(file_source == -1){
        printf("Error on open %s\n", argv[1]);
        return 1;
    }

    int flags = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    file_target = open(argv[2], flags, mode);
    if(file_target == -1){
        printf("Error on open %s\n", argv[2]);
        close(file_source);
        return 1;
    }

    while( (bytes_readed = read(file_source, buffer, BUFF_SIZE)) > 0 ){
        int i = 0;
        while(i < bytes_readed){
            if(buffer[i] != '\0'){
                int start_data = i;
                while(i < bytes_readed && buffer[i] != '\0') i++;
                int size_data = (i - start_data);
                write(file_target, &buffer[start_data], size_data);
            } else {
                int start_hole = i;
                while(i < bytes_readed && buffer[i] == '\0') i++;
                int size_hole = (i - start_hole);
                lseek(file_target, size_hole, SEEK_CUR);
            }
        } 
    }
    
    if(close(file_source) != 0 || close(file_target) != 0){
        printf("Error on close target or source file\n");
        return 1;
    }

    return 0;
}
