#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;
    size_t indexInInputHolder = 0;
    size_t currentStringLegth = 0;
    size_t indexInTable = 0;

    long offsetsFile_T[256];
    long stringLengthFile_T[256];

    char *inputHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(inputHolder == NULL){
        perror("There are problems with allocating memory with malloc.\n");
        exit(EXIT_FAILURE);
    }

    int stringNumber = -1;

    printf("Enter number of line, which you want to see: ");

    stringNumber = atoi(argv[2]);

    if(argc < 3){
        printf("Not enough arguments entered.");
    }

    fileDescriptorIn = open(argv[1], O_RDONLY, 0600);
    if(fileDescriptorIn == STATUS_FAIL){
        perror("There are problems while reading file.");
    }

    int readingStatus = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);

    if(readingStatus == STATUS_FAIL){
        perror("There are problems with reading file.");
    }

    while (readingStatus > 0){
        if(inputHolder[indexInInputHolder] == '\n'){
            currentStringLegth++;
            offsetsFile_T[indexInTable] = currentStringLegth;
            stringLengthFile_T[indexInTable++] = lseek(fileDescriptorIn, 0L, SEEK_CUR);
            indexInInputHolder = 0;
        } else {
            currentStringLegth++;
        }
        readingStatus = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);
    }
}
