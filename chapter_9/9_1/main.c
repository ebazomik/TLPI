#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/fsuid.h>
#include <stdio.h>
#include <unistd.h>

int main(){

		uid_t ruid, euid, suid, fsuid;
		
		int fs = setfsuid(0);
		int res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("initial\t-> ruid: %d euid: %d suid: %d fsid: %d\n\n", ruid, euid, suid, fsuid);
		}

		// unprivileged process	->	Change effective ID to the same value as current real or saved set ID.
		// privileged	->	Change real, effective, and saved set IDs to any (single) value.
		setuid(2000);
		fs = setfsuid(0);
		res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("setuid -> ruid: %d euid: %d suid: %d fsid: %d\n", ruid, euid, suid, fsuid);	
		}

		// unprivileged -> Change real ID to same value as current real or effective ID, and effective
		//								 ID to same value as current real, effective or saved set ID.
		// privileged -> Change real and effective IDs to any value.	
		setreuid(-1, 2000);
		fs = setfsuid(0);
		res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("setreuid -> ruid: %d euid: %d suid: %d fsid: %d\n", ruid, euid, suid, fsuid);	
		}

		// unprivileged	-> Change effective ID to the same value as current real or saved set ID. 
		// privileged	-> Change effective ID to any value.
		seteuid(2000);
		fs = setfsuid(0);
		res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("setuid -> ruid: %d euid: %d suid: %d fsid: %d\n", ruid, euid, suid, fsuid);	
		}

		// unprivileged	-> Change file-system ID to same value as current real, effective, file-system,
		//								 or saved set ID.
		// privileged	-> Change file-system ID to any value.
		setfsuid(2000);
		fs = setfsuid(0);
		res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("initial -> ruid: %d euid: %d suid: %d fsid: %d\n", ruid, euid, suid, fsuid);
		}

		setresuid(-1, 2000, 3000);
		fs = setfsuid(0);
		res = getresuid(&ruid, &euid, &suid);
		if(!res){
				printf("setresuid -> ruid: %d euid: %d suid: %d fsid: %d\n", ruid, euid, suid, fsuid);
		}

		return 0;
}




