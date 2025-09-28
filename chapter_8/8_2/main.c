/*	
 *	Copyright (C) 2025 ebazomik
 *
 *	Simple implementation of getpwnam.
 *
 *	This implementation uses getpwent and endpwent for iterate througth etc/passwd
 *	to find the correct user, returning pointer to struct * passwd or NULL if not found
 *  
 *	Licence: MIT
 */


#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

struct passwd * my_get_pwnam(char* user){

		struct passwd * pwd;
		while((pwd = getpwent()) != NULL){

				if(strcmp(pwd->pw_name, user) == 0){

						struct passwd * pass = malloc(sizeof(struct passwd));
						if(pass == NULL) return NULL;
						pass = pwd;
						return pass;

				}

		}

		endpwent();

		printf("%s not found\n", user);
		return NULL;
		
};



int main(int argc, char** argv){

		if(argc < 2){
				printf("Usage: ./program [user name]\n");
				return 1;
		}

		struct passwd * user_info = my_get_pwnam(argv[1]);

		if(user_info != NULL){

				printf("%p\n", user_info);
				printf("%s\n", user_info->pw_name);
				printf("%s\n", user_info->pw_passwd);
				printf("%d\n", user_info->pw_uid);
				printf("%d\n", user_info->pw_gid);
				printf("%s\n", user_info->pw_gecos);
				printf("%s\n", user_info->pw_dir);
				printf("%s\n", user_info->pw_shell);

		}

		free(user_info);

		

		return 0;
}
