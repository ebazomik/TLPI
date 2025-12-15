#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>


int main(){
	
	errno = 0;

	DIR * procDir = opendir("/proc/");
	if(procDir == NULL){
		printf("Error when try open /proc/ dir\n");
		return 1;
	}


	errno = 0;	
	struct dirent * process;

	while((process = readdir(procDir)) != NULL){
		if(errno != 0){
			printf("Error when read /proc/PID %s\n", strerror(errno));
			return 1;
		}

		
		char * ptrCheck;
		ssize_t realProcess = strtol(process->d_name, &ptrCheck, 10);
		
		if(*ptrCheck == '\0' && process->d_type == DT_DIR){
			
			char currFdPath[256];
			snprintf(currFdPath, 256, "/proc/%ld/fd/", realProcess);
			DIR * fdDir = opendir(currFdPath);
			if(fdDir == NULL){
				printf("Error when try open fd dir of %ld process\n", realProcess);
				return 1;
			}

			
			printf("PROC=[%ld]\n", realProcess);

			struct dirent * dirFd;
			errno = 0;
			while((dirFd = readdir(fdDir)) != NULL){
					if(errno != 0){
						printf("Error when read /proc/%ld/fd/: %s\n",realProcess, strerror(errno));
						return 1;
					}

					if(strcmp(dirFd->d_name, ".") == 0 || strcmp(dirFd->d_name, "..") == 0){
						continue;
					}

					ssize_t fdNumber = strtol(dirFd->d_name, &ptrCheck, 10);
					if(*ptrCheck == '\0' && dirFd->d_type == DT_LNK){

						char simPath[PATH_MAX];
						snprintf(simPath, sizeof(simPath), "/proc/%ld/fd/%ld", realProcess, fdNumber);

						errno = 0;
						char rdBuff[PATH_MAX];
						ssize_t nbytes;

						if((nbytes = readlink(simPath, rdBuff, sizeof(rdBuff)-1)) == -1){
							printf("Error on readlink: %s", strerror(errno));
							continue;
						}
						
						rdBuff[nbytes] = '\0';
						printf("(%ld) => %s\n", fdNumber, rdBuff);

					}
					
					errno = 0;
			}

			closedir(fdDir);
		}
	}


	closedir(procDir);

	return 0;
}
