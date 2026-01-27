#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define DEFAULT_LINE_READ 10
#define BUFFER_SIZE 4096

int main(int argc, char ** argv){

	int opt;
	int linesToRead = DEFAULT_LINE_READ;
	char *filePath = NULL;

	if(argc > 1){
		filePath = argv[1];
	} else {
		printf("Usage: %s [filepath] -option[-n num]\n", argv[0]);
		return 1;
	}

	while((opt = getopt(argc, argv, "n:")) != -1){
		switch(opt){
			case 'n':
				linesToRead = atoi(optarg);
				if(linesToRead == 0) linesToRead = DEFAULT_LINE_READ;
				break;
		}
	}

	char buffer[BUFFER_SIZE];

	int fd = open(filePath, O_RDONLY);
	if(fd == -1){
		printf("Error while try open filepath %s\n", filePath);
		return 1;
	}

	off_t fileSize = lseek(fd, 0, SEEK_END);
	if(fileSize == -1){
		printf("Error on seek: %s\n", strerror(errno));
		close(fd);
		return 1;
	}

	int bytesRemaining = fileSize;
	int newLinesFound = 0;
	off_t targetOffset = 0;

	while(bytesRemaining > 0 && newLinesFound < linesToRead){
		int chunk = (bytesRemaining > BUFFER_SIZE) ? BUFFER_SIZE : bytesRemaining;
		bytesRemaining = bytesRemaining - chunk;

		if(lseek(fd, bytesRemaining, SEEK_SET) == -1){
			printf("Error on seek: %s\n", strerror(errno));
			close(fd);
			return 1;
		};
		
		int bytesReaded = read(fd, buffer, chunk);
		if(bytesReaded != chunk){
			printf("Error reading chunk, expected: %d, readed: %d", chunk, bytesReaded);
			close(fd);
			return 1;
		}

		for(int i = chunk; i>=0; i--){
			if(buffer[i] == '\n'){
				newLinesFound++;
				if(newLinesFound == linesToRead +1){
					targetOffset = bytesRemaining + i + 1;
					if(lseek(fd, targetOffset, SEEK_SET) == -1){
						printf("Error on seek: %s\n", strerror(errno));
						close(fd);
						return 1;
					}
					break;
				}
			}
		}

	}

	if(newLinesFound == 0 || newLinesFound < linesToRead) {
		targetOffset = 0;
	} else if (targetOffset >= fileSize){
		targetOffset = fileSize;
	}


	lseek(fd, targetOffset, SEEK_SET);

	int bytesReaded = 0;

	while((bytesReaded = read(fd, buffer, BUFFER_SIZE)) > 0){
		write(STDOUT_FILENO, buffer, bytesReaded);
	}

	if(bytesReaded == -1){
		printf("Error while read fd %d\n %s\n", fd, strerror(errno));
		return 1;
	}

	close(fd);
	
	return 0;
}
