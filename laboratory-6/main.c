#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define MAX_NUMBER_OF_STRING 1000

int fillTable(long* offsetsFile_T, long* stringLengthFile_T, int fileDescriptorIn){

    char *inputHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(inputHolder == NULL){
        perror("There are problems with allocating memory with malloc.\n");
        exit(EXIT_FAILURE);
    }

    size_t indexInInputHolder = 0;
    size_t currentStringLength = 0;
    size_t indexInTable = 0;
    int readSymbols = -1;

    if( (readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE)) == STATUS_FAIL){
        perror("There are problems with reading file.");
        exit(STATUS_FAIL);
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
            exit(STATUS_FAIL);
        }
    }

    return indexInTable;
}

char* getStringByNumber(int fileDescriptorIn, long* offsetFile_T, const long* stringLengthFile_T, int stringsAmount){
    int stringNumber = -1;
    long currentBufferSize = INPUT_HOLDER_SIZE;
    int stopNumber = 0;
    int readSymbols = -1;
    int flag = 0;

    while(1){
        printf("There are %d strings.\n", stringsAmount);
        printf("Enter number of line, which you want to see: ");
        fflush(stdout);
        sleep(5);

        scanf("%d", &stringNumber);

        flag = fcntl(fileDescriptorIn, F_GETFL);
        printf("%d\n", flag);
        flag &= ~O_NONBLOCK;
        fcntl(fileDescriptorIn, F_SETFL, flag);
        printf("%d\n", flag);

        if(stringNumber < 0 || stringNumber >= MAX_NUMBER_OF_STRING){
            fprintf(stderr, "Invalid string number!");
        }

        if(stringNumber == stopNumber){
            printf("Stop number!\n");
            exit(EXIT_FAILURE);
        }

        lseek(fileDescriptorIn, offsetFile_T[stringNumber-1], SEEK_SET);

        currentBufferSize = stringLengthFile_T[stringNumber-1];
        char *stringHolder = (char*) malloc(currentBufferSize);
        if(stringHolder == NULL){
            perror("There are problems with allocating memory with malloc.\n");
            exit(EXIT_FAILURE);
        }

        if( ( readSymbols = read(fileDescriptorIn, stringHolder, stringLengthFile_T[stringNumber-1]-1) ) == STATUS_FAIL){
            perror("String number is invalid!");
            exit(STATUS_FAIL);
        }

        printf("%s\n", stringHolder);

    }
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;

    long offsetsFile_T[256];
    long stringLengthFile_T[256];

    if(argc < 2){
        printf("Not enough arguments entered.\n");
        exit(STATUS_FAIL);
    }

    if( (fileDescriptorIn = open(argv[1], O_RDONLY | O_NONBLOCK, 0600)) == STATUS_FAIL){
        perror("There are problems while reading file.");
        exit(STATUS_FAIL);
    }

    int stringsAmount = fillTable(offsetsFile_T, stringLengthFile_T, fileDescriptorIn);

    getStringByNumber(fileDescriptorIn, offsetsFile_T, stringLengthFile_T, stringsAmount);

    return EXIT_SUCCESS;
}
