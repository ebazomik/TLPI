#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int openDirOrCreate(char *path){
	DIR *dir = opendir(path);
	if(dir == NULL){
		int newDir = mkdir(path, 0755);
		if(newDir != 0){
			printf("Error on create new dir, %s don't exist\n", path);
			return -1;
		}
		return newDir;
	}
	return 0;
}

int createOneByteFile(char *path, int file){
	char buf[256];
	snprintf(buf, 256, "%s/%d", path, file);

	int fd = open(buf, O_CREAT | O_WRONLY, 0755);
	if(fd == -1){
		printf("Error on create file %d\n", file);
		return 1;
	}

	char c = 'A';
	int wr = write(fd, &c, 1);
	if(wr != 1){
		printf("File %d is empty. Nothing was writed\n", file);
		return 1;
	}

	close(fd);
	return 0;
}

int populateBuffer(int* buffer, int length){
	if(buffer == NULL) return 1;
	for(int i = 0; i < length -1; i++){
		buffer[i] = i;
	}
}

// [bufferShuffle] use knuth shuffle (Fisher-Yates)
// for shuffle buffer values
int bufferShuffle(int* buffer, int length){
	if(buffer == NULL) return 1;
	srand(time(NULL));
	for(int i = length -1; i >= 0; i--){
		int r = rand() % length;
		int temp = buffer[i];
		buffer[i] = buffer[r];
		buffer[r] = temp;
	}
} 

int removeFile(int fd){}
void generateFiles(int n, char *path){}
void removeAllFiles(char *path){}



int main(int argc, char ** argv){

	if(argc < 5) {
		printf("Usage: -d [directory path] -n [file number]\n");
		return 1;
	}

	int opt = -1;
	long quantity = -1;
	char * path = NULL;

	while((opt = getopt(argc, argv, "d:n:")) != -1){
		switch(opt){
			case 'd':
				path = optarg;
				break;
			case 'n':
				quantity = strtol(optarg, NULL, 10);
				break;
		}
	}

	if(quantity == -1 || path == NULL){
		printf("Invalid quantity or directory path\n");
		return 1;
	}
	
	int dir = openDirOrCreate(path);
	if(dir == -1){
		printf("Error on openDirOrCreate, %s don't exist\n", path);
		return 1;
	}

	int buffer[quantity];
	if(populateBuffer(buffer, quantity) == -1){
		printf("Error on populate buffer\n");
		return 1;
	}

	if(bufferShuffle(buffer, quantity) == -1){
		printf("Error on shuffle buffer\n");
		return 1;
	}

	for(int i = 0; i < quantity -1; i++){
		createOneByteFile(path, i);
	}

	return 0;
}
