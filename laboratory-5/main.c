#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define MAX_NUMBER_OF_STRING 1000

int fillTable(long* offsetsFile_T, long* stringLengthFile_T, char* inputHolder, int fileDescriptorIn, int readSymbols){

    size_t indexInInputHolder = 0;

    size_t currentStringLength = 0;
    size_t indexInTable = 0;

    if( (readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE)) == STATUS_FAIL){
        perror("There are problems with reading file.");
    }

    while (readSymbols > 0){

        while(indexInInputHolder < readSymbols){

            currentStringLength++;

            if(inputHolder[indexInInputHolder] == '\n'){

                offsetsFile_T[indexInTable] = indexInInputHolder + 1 - currentStringLength;
                stringLengthFile_T[indexInTable++] = currentStringLength;

                currentStringLength = 0;
            }

            indexInInputHolder++;

        }

        if( (readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE)) == STATUS_FAIL){
            perror("There are problems with reading file.");
        }

    }

    return indexInTable;

}

char* getStringByNumber(int fileDescriptorIn, long* offsetFile_T, const long* stringLengthFile_T, char* inputHolder){
    int stringNumber = -1;
    size_t indexInInputHolder = 0;
    long currentBufferSize = INPUT_HOLDER_SIZE;

    while(printf("Enter number of line, which you want to see: ") && scanf("%d", &stringNumber)){
        if(stringNumber < 0 || stringNumber >= MAX_NUMBER_OF_STRING){
            fprintf(stderr, "Invalid string number!");
        }

        if(stringNumber == 0){
            printf("Stop number!");
            exit(EXIT_FAILURE);
        }

        lseek(fileDescriptorIn, offsetFile_T[stringNumber+1], SEEK_CUR);

        if( stringLengthFile_T[stringNumber+1] < currentBufferSize){
            currentBufferSize = stringLengthFile_T[stringNumber+1];
            //realloc(inputHolder, currentBufferSize);
        }
    }
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;
    int readSymbols;

    long offsetsFile_T[256];
    long stringLengthFile_T[256];

    char *inputHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(inputHolder == NULL){
        perror("There are problems with allocating memory with malloc.\n");
        exit(EXIT_FAILURE);
    }

    if(argc < 2){
        printf("Not enough arguments entered.\n");
    }

    if( (fileDescriptorIn = open(argv[1], O_RDONLY, 0600)) == STATUS_FAIL){
        perror("There are problems while reading file.");
    }

    int stringsAmount = fillTable(offsetsFile_T, stringLengthFile_T, inputHolder, fileDescriptorIn, readSymbols);

    for (int i = 0; i < stringsAmount; ++i) {
        printf("%lu ", offsetsFile_T[i]);
        printf("%lu\n", stringLengthFile_T[i]);
    }

    //getStringByNumber(inputHolder);

    return EXIT_SUCCESS;
}
