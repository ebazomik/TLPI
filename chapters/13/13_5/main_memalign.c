#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define DEFAULT_LINE_READ 10
#define BUFFER_SIZE 4096

int openFileAndDefineMode(char *filePath, int *fd, int *blockSize, int *fileSize){

	struct stat statFile;
	int statRes = stat(filePath, &statFile);
	if(statRes != 0){
		printf("Error calling stat: %s\n", strerror(errno));
		return -1;
	}

	*blockSize = statFile.st_blksize;
	*fileSize = statFile.st_size;
	int directMode = *fileSize > *blockSize;

	int flags = O_RDONLY;
	if(directMode) flags |= O_DIRECT;
	*fd = open(filePath, flags);
	if(*fd == -1){
		printf("Error trying open %s in %s mode\n", filePath, directMode ? "O_DIRECT" : "CACHED");
		return -1;
	}

	return directMode ? 1 : 2;

}

int findLineOffset(int fd, int mode, char *buffer, int fileSize, int blockSize, int linesToRead){

	int isDirectMode = mode == 1;
	off_t offset;
	int bytesToRead;
	int isFirstRead = 1;

	if(isDirectMode){
		offset = (fileSize / blockSize) * blockSize;
		bytesToRead = blockSize;
	} else {
		if(fileSize > blockSize){
			offset = fileSize - blockSize;
			bytesToRead = blockSize;
		} else {
			offset = 0;
			bytesToRead = fileSize;
		}

	}

	int linesFounded = 0;

	if(lseek(fd, offset, SEEK_SET) == -1){
		printf("Error on seek: %s\n", strerror(errno));
		return -1;
	};	

	while(offset >= 0 && linesFounded < linesToRead){
		
		if(lseek(fd, offset, SEEK_SET) == -1){
			printf("Error on seek: %s\n", strerror(errno));
			return -1;
		};


		int actualBytesToRead;
		if(isDirectMode){
			actualBytesToRead = blockSize;
		} else {
			actualBytesToRead = bytesToRead;
		}

		int bytesReaded = read(fd, buffer, actualBytesToRead);

		if(bytesReaded <= 0){
			printf("Error while reading in %s mode, expected: %d, readed: %d\n", isDirectMode ? "O_DIRECT" : "CACHED => %s\n", bytesToRead, bytesReaded, strerror(errno));
			return -1;
		}

		int startIndex = bytesReaded -1;

		if(isFirstRead && offset + bytesReaded >= fileSize && startIndex >= 0 && buffer[startIndex] == '\n'){
			startIndex--;
		}

		isFirstRead = 0;

		for(int i = startIndex -1; i >= 0; i--){
			if(buffer[i] == '\n'){
				linesFounded++;
				if(linesFounded == linesToRead){
					offset = offset + i + 1;
					break;
				}
			}
		}

		if(linesFounded == linesToRead) break;

		offset -= blockSize;

	}

	if(linesFounded < linesToRead){
		offset = 0;
	}
	
	return offset;
}

int printLines(int fd, char *buffer, int startOffset, int blockSize, int isDirectMode){

	off_t currentOffset = startOffset;
	int skipBytes = 0;

	int needToAlignBlock = startOffset % blockSize != 0;
	if(isDirectMode && needToAlignBlock){
		currentOffset = (startOffset / blockSize) * blockSize;
		skipBytes = startOffset - currentOffset;
	}

	if (lseek(fd, currentOffset, SEEK_SET) == -1) {
		fprintf(stderr, "Error: seek failed: %s\n", strerror(errno));
    return -1;
  }

	int bytesReaded = 0;

	int isFirstRead = 1;
	while((bytesReaded = read(fd, buffer, blockSize)) > 0){

		if(isFirstRead && skipBytes > 0){
			write(STDOUT_FILENO, buffer + skipBytes, bytesReaded - skipBytes);
			isFirstRead = 0;
		} else {
			write(STDOUT_FILENO, buffer, bytesReaded);
		}

	}
	
	
	if(bytesReaded == -1){
		printf("Error while read fd %d\n %s\n", fd, strerror(errno));
		return -1;
	}

	return 0;
}


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

	int fd = -1;
	int blockSize = 0;
	int fileSize = 0;
	char * buffer = NULL;

	int mode = openFileAndDefineMode(filePath, &fd, &blockSize, &fileSize);
	if(mode == -1){
		close(fd);
		return 1;
	}

	if(mode == 1){
		int res = posix_memalign((void **)&buffer, blockSize, blockSize);
		if(res != 0 || buffer == NULL){
			printf("Error posix_memaling: %s\n", strerror(errno));
			close(fd);
			return 1;
		}
	} else {
		buffer = malloc(blockSize);
		if(buffer == NULL){
			printf("Error malloc: %s\n", strerror(errno));
			close(fd);
			return 1;
		}
	}

	int offset = findLineOffset(fd, mode, buffer, fileSize, blockSize, linesToRead);
	if(offset == -1){
		close(fd);
		free(buffer);
		return 1;
	}

	int printResult = printLines(fd, buffer, offset, blockSize, mode == 1);
	if(printResult == -1){
		close(fd);
		free(buffer);
		return 1;
	}

	close(fd);
	free(buffer);
	
	return 0;
}
