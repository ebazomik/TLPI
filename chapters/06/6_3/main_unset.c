#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char ** environ;

int my_unsetenv(char * key){

		char ** envcopy = environ;
		int counter = 0;

		for(int i = 0; envcopy[i] != NULL; i++){	
				char * env = envcopy[i];
				char currentKey[50] = "";

				for(int j = 0; env[j] != '=' && env[j] != '\0'; j++){
						char toAdd[2] = { env[j] , '\0' };
						strcat(currentKey, toAdd);
				}

				if(!strcmp(currentKey, key)){
						
						for(int k = i; envcopy[k] != NULL; k++){
								envcopy[k] = envcopy[k+1];
						}
						i--;
						counter++;
				}
		}

		printf("Removed %d variables\n", counter);
		return 0;

}


int main(int argc, char ** argv, char ** envp){

		if(argc < 2){
				printf("Usage: [keyenv]\n");
				return 1;
		}
	
		printf("Before remove %s=%s\n", argv[1], getenv(argv[1]));

		if(my_unsetenv(argv[1])){
				printf("Eerror my_unsetenv\n");
				return 1;
		};

		printf("After remove: %s=%s\n", argv[1], getenv(argv[1]));

		return 0;

}
