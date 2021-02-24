#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define STATUS_FAIL -1

int main(int argc, char* argv[]) {
    int fileDescriptorIn = 0;

    char *inputHolder = (char*)malloc(1024);
    if(inputHolder == NULL){
        perror("There are problems with allocating memory with malloc.\n");
        exit(EXIT_FAILURE);
    }

    if( (fileDescriptorIn = open(argv[1], O_RDONLY | O_NONBLOCK, 0600)) == STATUS_FAIL){
        perror("There are problems while reading file.");
        exit(STATUS_FAIL);
    }

    printf("Check\n");
    fflush(stdout);
    sleep(5);

    if (read(0, inputHolder, 1024) == 0){
        printf("Ops...");
    }

    int flag = fcntl(fileDescriptorIn, F_GETFL);
    printf("%d\n", flag);
    flag &= ~O_NONBLOCK;
    fcntl(fileDescriptorIn, F_SETFL, flag);
    printf("%d\n", flag);

    return EXIT_SUCCESS;
}

