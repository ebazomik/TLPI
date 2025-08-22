/*  With this execise verify the flag O_APPEND.
 *  O_APPEND grants the atomicity of opened file to prevent overwrite data in a multi-process context.
 *  With O_APPEND, before every call to write the offset of file is set to end of existing data. 
 *  Also try getopt for parse input arguments.
 *  */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

    int opt;
    char *filePath = NULL;
    char *inputData = NULL; 

    if(argc < 5){
        printf("Usage: %s -f [filepath] -s [data to add] \n", argv[0]);
        return 1;
    }

    while((opt = getopt(argc, argv, "f:s:")) != -1){
        switch(opt){
            case 'f':
                filePath = optarg;
                break;
            case 's':
                inputData = optarg;
                break;
            default:
                printf("Usage: %s -f [filePath] -s [data to add] \n", argv[0]);
                return 1;
                break;
       }
    }

    if(filePath == NULL || inputData == NULL){
        printf("File path -f or data -s cannot be empty \n");
        return 1;
    }

    int fd;
    int flags = O_WRONLY | O_APPEND;
    fd = open(filePath, flags);
    if(fd == -1){
        printf("Error on open file %s \n", filePath);
        return 1;
    }
    
    // Set offset to beginning before write
    lseek(fd, 0, SEEK_SET);
    
    ssize_t written;
    // Whit O_APPEND the lseek is performed again and set offset to end of existing data lseek(fd, 0, SEEK_END)
    written = write(fd, inputData, strlen(inputData));
    if(written == -1){
        printf("Error on write data to file\n");
        return 1;
    }

    close(fd);

    return 0;
}
