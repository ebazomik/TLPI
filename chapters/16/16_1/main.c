#include <stdio.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    
    int opt = 0;
    int remove = 0;
    char *filepath = NULL;
    char* attr = NULL;
    char* value = NULL;

    while((opt = getopt(argc, argv, "f:n:v:x")) !=  -1){
        switch(opt){
            case 'f':
                filepath = optarg;
                break;
            case 'n':
                attr = optarg;
                break;
            case 'v':
                value = optarg;
                break;
            case 'x':
                remove = 1;
                break;
        }
    }

    if( filepath == NULL || attr == NULL){
        fprintf(stderr, "Error: -f [filepath] and -n [name] are required\n");
        return 1;
    }

    if(remove){
        int res = removexattr(filepath, attr);
        if(res == -1){
            perror("removexattr");
            return 1;
        }
        printf("Removed attribute %s from %s\n", attr, filepath);

    } else {
        if(value == NULL){
            fprintf(stderr, "Errore: -v [value] is required when setting attribute\n");
            return 1;
        }

        int res = setxattr(filepath, attr, value, strlen(value), 0);
        if(res == -1){
            perror("setxattr");
            return 1;
        }
        printf("Setted attribute %s = %s in %s\n", attr, value, filepath);
    }
    return 0;
}
