#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_CHILDREN 256

struct process_node {
	pid_t pid;
	pid_t ppid;
	char cmdline[256];
	char pname[256];
	struct process_node * children[MAX_CHILDREN];
	int children_count;
};


/* Read bytes until meet new line (\n) */
size_t read_line(int fd, char * buffer_line, size_t size){
	if(size == 0) return 0;

	size_t bytes_read = 0;
	char ch = 0;

	while(bytes_read < size -1){
		ssize_t ch_read = read(fd, &ch, 1);
		if(ch_read <= 0) break;
		
		if(ch == '\n'){
			buffer_line[bytes_read] = '\0';
			return bytes_read;
		}

		buffer_line[bytes_read++] = ch;
	}

	buffer_line[bytes_read] = '\0';
	return bytes_read;
}

/* Return pid_t valure for recover pid and ppid */
pid_t get_value(char * buffer, char * key){
	if(strncmp(buffer, key, strlen(key)) == 0){
		char * token = strtok(buffer, ":");
		token = strtok(NULL, ":");
		if(token != NULL){
			while(*token == ' ' || *token == '\t') token++;
			return atoi(token);
		}
	}

	return -1;
}

/* Iterate nodes whit deep first search (dfs) and print formatted info of parent and childs */
void dfs(struct process_node * list, int count){
	for(int i = 0; i < count; i++){
		int children_count = 0;
		printf("[%d] => %s\n", list[i].pid, list[i].cmdline);

		while(list[i].children[children_count] != NULL){
			printf("  ∟ [%d] => %s\n", list[i].children[children_count]->pid, list[i].children[children_count]->cmdline);
			children_count++;
		}

		printf("\n\n");
	}	
}



int main(){

	errno = 0;
	struct rlimit rlim;
	if(getrlimit(RLIMIT_NPROC, &rlim) == -1){
		printf("Error on get RLIMIT_NPROC: %s\n ", strerror(errno));
		rlim.rlim_cur = 4096;
	}
	
	// Initialize process list size with RLIMIT_NPROC
	size_t plimit = rlim.rlim_cur;
	struct process_node *process_list = malloc(plimit * sizeof(struct process_node));

	if(process_list == NULL){
		printf("Error: try to allocate process_list with RLIMIT_NPROC\n");
		return 1;
	}

	errno = 0;
	DIR * proc = opendir("/proc/");
	if(proc == NULL){
		printf("Error on open /proc/: %s\n", strerror(errno));
		free(process_list);
		return 1;
	}

	struct dirent * process;
	errno = 0;
	int pcount = 0;

	while((process = readdir(proc)) != NULL){
		if(errno != 0){
			printf("Error on read procces in /proc/: %s\n", strerror(errno));
			free(process_list);
			return 1;
		}
	
		/* Get info only if is a valid process */
		errno = 0;
		char * endptr;
		long valid_process = strtol(process->d_name, &endptr, 10);

		if(process->d_type == DT_DIR && *endptr == '\0' && valid_process){

			struct process_node node;
			node.pid = -1;
			node.ppid = -1;
			node.cmdline[0] = '\0';
			node.children_count = 0;

			for(int i = 0; i < MAX_CHILDREN; i++) {
				node.children[i] = NULL;
      }

			char process_path_status[PATH_MAX];
			snprintf(process_path_status, sizeof(process_path_status), "/proc/%s/status", process->d_name);

			int fd_status = open(process_path_status, O_RDONLY);
			if(fd_status == -1){
				printf("Error on open process %s status file\n", process->d_name);
				continue;
			}

			char line[256];
			size_t readed;

			while((readed = read_line(fd_status, line, sizeof(line))) > 0){

				if(strstr(line, "Pid:") != NULL){
					pid_t value = get_value(line, "Pid:");
					if(value != -1){
						node.pid = value;
					}
				}

				if ( strstr(line, "PPid:") != NULL){	
					pid_t value = get_value(line, "PPid:");
					if(value != -1){
						node.ppid = value;
					}
				}
				
				if(node.pid != -1 && node.ppid != -1){
					break;
				}
			}

			close(fd_status);
			
			if(node.pid != -1){
				char process_path_cmdline[PATH_MAX];
        snprintf(process_path_cmdline, sizeof(process_path_cmdline), "/proc/%s/cmdline", process->d_name);

        int fd_cmdline = open(process_path_cmdline, O_RDONLY);
        if(fd_cmdline != -1){
					char line_cmdline[256];
          ssize_t readed_cmdline = read_line(fd_cmdline, line_cmdline, sizeof(line_cmdline) - 1);

          if(readed_cmdline > 0){
						line_cmdline[readed_cmdline] = '\0';
                    
            for(ssize_t i = 0; i < readed_cmdline; i++){
							if(line_cmdline[i] == '\0') line_cmdline[i] = ' ';
            }
                    
            strncpy(node.cmdline, line_cmdline, sizeof(node.cmdline) - 1);
            node.cmdline[sizeof(node.cmdline) - 1] = '\0';
          }
          
					close(fd_cmdline);
        }

        process_list[pcount] = node;
        pcount++;
      }
    }

		errno = 0;
	}

	closedir(proc);

	// Iterate list and connect children to parent
	for(int i = 0; i<pcount; i++){
		struct process_node * parent = &process_list[i];
		for(int j = 0; j<pcount; j++){
			struct process_node * child = &process_list[j];
			if(child->ppid == parent->pid && parent->children_count < MAX_CHILDREN){
				parent->children[parent->children_count] = child;
				parent->children_count++;
			}
		}
	}

	/* Print formatted data */
	dfs(process_list, pcount);
	free(process_list);


	return 0;
}




