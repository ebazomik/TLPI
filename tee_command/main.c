#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]){
    char input_buf[BUF_SIZE];
    char file_path[BUF_SIZE];
    ssize_t bytes_written, bytes_readed;
    int fd_open, append, open_flags, open_perms;

    if(argc < 2 || argc > 3){
        printf("Only 2 parameters accepted: %s -a [optional] filepath\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "-a")){
        append = 0;
        strncpy(file_path, argv[1], BUF_SIZE -1);
        file_path[BUF_SIZE - 1] = '\0';
    } else {
        append = 1;
        strncpy(file_path, argv[2], BUF_SIZE -1);
        file_path[BUF_SIZE - 1] = '\0';
    }
    
    // Limited to 1024 byte input, for read more use while(read) until you meet 0;
    bytes_readed = read(STDIN_FILENO, input_buf, sizeof(input_buf) -1);

    if(bytes_readed == -1){
        printf("Error on reading input %s\n", strerror(errno));
        return 1;
    }

    if(bytes_readed > 0){
        input_buf[bytes_readed] = '\0';
        bytes_written = write(STDOUT_FILENO, input_buf, strlen(input_buf));
        if(bytes_written == -1){
            printf("Error on write stdout %s\n", strerror(errno));
            return 1;
        } 
    }

    if(strlen(file_path) < 0) {
        printf("File destination missing: %s [ -a ] | [ filepath ]\n", argv[0]);
        return 1;
    }

    open_flags = append ? ( O_RDWR | O_APPEND | O_CREAT ) : ( O_RDWR | O_TRUNC | O_CREAT );
    open_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    fd_open = open(file_path, open_flags, open_perms);

    if(fd_open == -1){
        printf("Error on open file\n");
        printf("%s\n", strerror(errno));
        return 1;
    }

    if(write(fd_open, input_buf, strlen(input_buf)) == -1){
        printf("Error on write on %s\n", file_path);
        printf("%s\n", strerror(errno));
        return 1;
   }

    if(close(fd_open) == -1){
        printf("Error on close file %s\n", file_path);
        return 1;
    }
    
    return 0;
}
