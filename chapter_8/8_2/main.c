#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>

struct passwd * my_get_pwnam(char* user){

		struct passwd * pwd;
		while((pwd = getpwent()) != NULL){

				if(strcmp(pwd->pw_name, user) == 0){

						struct passwd * pass = malloc(sizeof(struct passwd));
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

		return 0;
}
