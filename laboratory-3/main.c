#include <stdlib.h>
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

int main(int argc, char* argv[]){
    FILE* file = NULL;

    file = fopen("test.txt", "r");

    printf("Real user ID: %d\nEffective user ID: %d\n\n", getuid(), geteuid());

    if(file == NULL){
        perror("There are problems with opening file. (First try)");
    } else {
        printf("File is opened. (First try)\n\n");
        int closingFileStatus = fclose(file);
        if(closingFileStatus != STATUS_SUCCESS){
            perror("There are problems with closing file.");
            exit(EXIT_FAILURE);
        }
    }

    int settingUIDStatus = setuid(getuid());

    if(settingUIDStatus == STATUS_FAIL){
        perror("There are problems with setting UID.");
        exit(EXIT_FAILURE);
    }

    file = fopen("test.txt", "r");

    printf("New real user ID: %d\nNew effective user ID: %d\n\n", getuid(), geteuid());

    if(file == NULL){
        perror("There are problems with opening file. (Second try)");
        exit(EXIT_FAILURE);
    }

    printf("File is opened. (Second try)\n\n");
    int closingFileStatus = fclose(file);
    if(closingFileStatus != STATUS_SUCCESS){
        perror("There are problems with closing file.");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}