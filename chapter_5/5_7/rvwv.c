#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 10

int main(int argc, char ** argv){

    
    if(argc != 2){
        printf("Usage: program [file]\n");
        return 1;
    }

    int fd = open(argv[1], O_RDWR);

    if(fd == -1){
        printf("Error on open %s\n", argv[1]);
        return 1;
    }

    char buffer[BUFF_SIZE];
    char buffer1[BUFF_SIZE];
    char buffer2[BUFF_SIZE];

    struct iovec iov_rw[3];

    iov_rw[0].iov_base = buffer;
    iov_rw[0].iov_len = BUFF_SIZE;
    iov_rw[1].iov_base = buffer1;
    iov_rw[1].iov_len = BUFF_SIZE;
    iov_rw[2].iov_base = buffer2;
    iov_rw[2].iov_len = BUFF_SIZE;

    
    ssize_t total_read = 0;
    ssize_t total_write = 0;
    int tot_iov = 3;

    for(int i = 0; i < tot_iov; i++){
        ssize_t bytes_readed = read(fd, iov_rw[i].iov_base, iov_rw[i].iov_len);

        if(bytes_readed == -1){
            printf("Error on reading bytes at iovec index [%d] buffer\n", i);
            return 1;
        }

        total_read += bytes_readed;

        ssize_t bytes_written = write(STDOUT_FILENO, iov_rw[i].iov_base, iov_rw[i].iov_len);

        if(bytes_written == -1){
            printf("Error on writeing bytes at iovec index [%d] buffer\n", i);
            return 1;
        }

        total_write += bytes_written;    

    }

    printf("\n");
    printf("Total bytes readed [%ld] and writed [%ld]\n", total_read, total_write);

    close(fd);

    return 0;
}
