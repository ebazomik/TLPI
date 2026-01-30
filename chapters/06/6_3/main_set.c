#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


int my_setenv(char * name, char * value, int overwrite){
		if(getenv(name) != NULL && !overwrite) return 0;

		int size = strlen(name) + strlen(value) + 2;
		int * p = malloc(size);

		if(p == NULL){
				errno = ENOMEM;
				return -1;
		}

		snprintf((char *)p, size, "%s=%s", name, value);
		int pe = putenv((char *)p);
		if(pe != 0){
				free(p);
				return -1;
		}

		return 0;
}


int main(int argc, char ** argv){
				
				if(argc < 3){
								printf("Usege params: [char * name] [ char * value]\n");
								return 1;
				}

				int eset = my_setenv(argv[1], argv[2], 1);
				if(eset){
						printf("Error my_setenv code:%d\n", eset);
						printf(strerror(errno));
						return 1;
				}

				char * v = getenv(argv[1]);
				if(v == NULL){
								printf("%s not found\n", argv[1]);
								return 1;
				}
				printf("Env variable set: %s\n", v);

				return 0;

}
