#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

uid_t userNameFromId(const char * name){
	struct passwd * pwd;
	uid_t userId;
	char * endptr;
	
	if(name == NULL || * name == '\0'){
		return -1;
	}

	userId = strtol(name, &endptr, 10);
	if(* endptr == '\0'){
		return userId;
	}

	pwd = getpwnam(name);
	if(pwd == NULL){
		return -1;
	}

	return pwd->pw_uid;
}


size_t readLine(int fd, char bufferLine[], size_t size){
	if(size == 0) return -1;

	size_t bytesRead = 0;
	char ch;
	
	while(bytesRead < size - 1){
		ssize_t chRead = read(fd, &ch, 1);
		if(chRead <= 0){
			if(chRead == 0){
				return -1;
			}
			break;
		}

		if(ch == '\n'){
			bufferLine[bytesRead] = '\n';
			return bytesRead;
		}

		bufferLine[bytesRead++] = ch; 
	}

	bufferLine[bytesRead] = '\n';
	return bytesRead;
}

uid_t getUid(char buffer[]){
	char * token = strtok(buffer, "\t");
	token = strtok(NULL, "\t");

	if(token != NULL){
		uid_t procId = strtol(token, NULL, 10);
		return procId;
	}
}



int main(int argc, char ** argv){
	
	if(argc < 2){
		printf("Usage: ./pidAndCommand [user]\n");
		return 1;
	}

	uid_t userId;
	struct dirent * process;
	DIR * procDir;

	userId = userNameFromId(argv[1]);

	if(userId == -1){
		printf("Error on read id from %s\n", argv[1]);
		return 1;
	}

	procDir = opendir("/proc/");
	if(procDir == NULL){
		printf("Error on open /proc/ folder\n");
		return 1;
	}

	while((process = readdir(procDir)) != NULL){
		
		if(process->d_type == DT_DIR){

			char path[256];
			snprintf(path, sizeof(path), "/proc/%s/status", process->d_name);
			int fd = open(path, O_RDONLY);

			if(fd != -1){
				char buffer[256];

				while(readLine(fd, buffer, sizeof(buffer)) != -1){

					if(strstr(buffer, "Uid:") != NULL){
						uid_t procId = getUid(buffer);

						if(userId == procId){
							char commandPath[256];
							snprintf(commandPath, sizeof(commandPath), "/proc/%s/cmdline", process->d_name);
							int fdCommand = open(commandPath, O_RDONLY);

							if(fdCommand != -1){
								char commandBuffer[256];
								readLine(fdCommand, commandBuffer, sizeof(commandBuffer));
								printf("process\t\tcommand\n");
								printf("%s\t\t%s\n\n", process->d_name, commandBuffer);
							}

							close(fdCommand);
						}
						break;
					}	
				}
			}

			close(fd);
		}

	}

	closedir(procDir);
	return 0;
}
