#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define MAX_NUMBER_OF_STRING 1000
#define OWNER_READ_WRITE 0600
#define STDIN 0

extern int errno;

long getStringNumber(){
    char *numberHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(numberHolder == NULL){
        perror("There are problems with allocating memory with malloc");
        return STATUS_FAIL;
    }
    int readSymbols = read(STDIN, numberHolder, INPUT_HOLDER_SIZE);
    if( readSymbols == STATUS_FAIL){
        perror("There are problems with reading file");
        free(numberHolder);
        return STATUS_FAIL;
    }

    long stringNumber = strtol(numberHolder, NULL, 10);
    if(stringNumber == 0 && errno != STATUS_SUCCESS){
        perror("There are troubles with converting string to long");
        return STATUS_FAIL;
    }

    return stringNumber;
}

int fillTable(long* offsetsFile_T, long* stringLengthFile_T, int fileDescriptorIn){

    char *inputHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(inputHolder == NULL){
        perror("There are problems with allocating memory with malloc");
        return STATUS_FAIL;
    }

    size_t indexInInputHolder = 0;
    size_t currentStringLength = 0;
    size_t indexInTable = 0;
    int readSymbols = -1;

    readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);
    if( readSymbols == STATUS_FAIL){
        perror("There are problems with reading file");
        free(inputHolder);
        return STATUS_FAIL;
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
        readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);
        if( readSymbols == STATUS_FAIL){
            perror("There are problems with reading file");
            free(inputHolder);
            return STATUS_FAIL;
        }
    }

    free(inputHolder);

    return indexInTable;
}

int printStringByNumber(int fileDescriptorIn, long* offsetFile_T, const long* stringLengthFile_T, int stringsAmount){
    long currentBufferSize = INPUT_HOLDER_SIZE;
    long stopNumber = 0;
    int readSymbols = -1;
    long stringNumber = 1;
    int status = 0;

    while(1){

        printf("There are %d strings.\nEnter number of line, which you want to see: ", stringsAmount);
        fflush(stdout);

        stringNumber = getStringNumber();

        if(stringNumber == 0){
            printf("Stop number!\n");
            break;
        }

        if(stringNumber < 0 || stringNumber > stringsAmount){
            fprintf(stderr, "Invalid string number!\n");
            fflush(stdout);
            continue;
        }

        status = lseek(fileDescriptorIn, offsetFile_T[stringNumber-1], SEEK_SET);
        if(status == STATUS_FAIL){
            perror("There are problems with setting position in file");
            // EBADF, ESPIPE, EINVAL, EOVERFLOW, ENXIO - Ошибки lseek
            return STATUS_FAIL;
        }

        currentBufferSize = stringLengthFile_T[stringNumber-1];
        char *stringHolder = (char*) malloc(currentBufferSize);
        if(stringHolder == NULL){
            perror("There are problems with allocating memory with malloc");
            return STATUS_FAIL;
        }

        readSymbols = read(fileDescriptorIn, stringHolder, stringLengthFile_T[stringNumber-1]-1);

        if(readSymbols == STATUS_FAIL){
            perror("String number is invalid");
            free(stringHolder);
            return STATUS_FAIL;
        }

        printf("%s\n", stringHolder);
        free(stringHolder);
    }

    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;

    long offsetsFileTable[256];
    long stringLengthFileTable[256];
    int status;

    if(argc < 2){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file");
        exit(EXIT_FAILURE);
    }

    if( (fileDescriptorIn = open(argv[1], O_RDONLY, OWNER_READ_WRITE)) == STATUS_FAIL){
        perror("There are problems while reading file.");
        exit(EXIT_FAILURE);
    }

    int stringsCount = fillTable(offsetsFileTable, stringLengthFileTable, fileDescriptorIn);

    if(stringsCount == STATUS_FAIL){
        fprintf(stderr, "There are problems while filling table.");
        return EXIT_FAILURE;
    }

    status = printStringByNumber(fileDescriptorIn, offsetsFileTable, stringLengthFileTable, stringsCount);

    if(status == STATUS_FAIL) {
        fprintf(stderr, "There are problems while printing string by number.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
