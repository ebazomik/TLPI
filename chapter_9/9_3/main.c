#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <sys/types.h> 
#include <errno.h>

int main(int argc, char * argv[]){

		if(argc < 2){
				printf("Usage: ./program [user]\n");
				return 1;
		}

		struct group * grp = NULL;
		gid_t * grpIdsList = NULL;
		int count = 0;
		int capacity = 10;

		setgrent();

		grpIdsList = malloc(capacity * sizeof(gid_t));
		if(grpIdsList == NULL){
				printf("error on stat malloc\n");
				return 1;
		}

		while((grp = getgrent()) != NULL){
				char ** mem = grp->gr_mem;
				while(*mem != NULL){
						if(!strcmp(*mem, argv[1])){
								if(count >= capacity){
										capacity = capacity * 2;
										gid_t * temp = realloc(grpIdsList, capacity * sizeof(gid_t));
										if(temp == NULL){
												printf("can't realloc\n");
												return 1;
										}
										grpIdsList = temp;
								}
								grpIdsList[count] = grp->gr_gid;
								count++;
								break;
						}
						mem ++;
				}
		}

		if(setgroups(count, grpIdsList) == -1){
				printf("Error on setGroups, remember that you need root privileges for setgroup\n");
				printf("%s\n", strerror(errno));
				return 1;
		}


		endgrent();
		free(grpIdsList);

		return 0;
}
