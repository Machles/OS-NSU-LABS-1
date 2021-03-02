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
/// Объясните почему нельзя использовать scanf?

long getStringNumber(int stringsCount){
    char *endptr = NULL;
    printf("There are %d strings.\nEnter number of line, which you want to see: ", stringsCount);
    int status = fflush(stdout);
    if(status != STATUS_SUCCESS){
        perror("There are problems while getting your number, exactly with fflush command");
        return STATUS_FAIL;
    }

    char *numberHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(numberHolder == NULL){
        perror("There are problems while getting your number, exactly with allocating memory with malloc");
        return STATUS_FAIL;
    }
    int readSymbols = read(STDIN, numberHolder, INPUT_HOLDER_SIZE);
    if(readSymbols == STATUS_FAIL){
        perror("There are problems while getting your number, exactly with reading file");
        free(numberHolder);
        return STATUS_FAIL;
    }

    long stringNumber = strtol(numberHolder, &endptr, 10);
    if(stringNumber == 0 && endptr != NULL){
        fprintf(stderr, "There are problems while getting your number, exactly with converting string to long.");
        free(numberHolder);
        return STATUS_FAIL;
    }

    return stringNumber;
}

int fillTable(long* offsetsFileTable, long* stringsLengthsFileTable, int fileDescriptorIn){

    char *inputHolder = (char*)malloc(INPUT_HOLDER_SIZE);
    if(inputHolder == NULL){
        perror("There are problems while filling table, exactly with allocating memory with malloc");
        return STATUS_FAIL;
    }

    size_t indexInInputHolder = 0;
    size_t currentStringLength = 0;
    size_t indexInTable = 0;
    int readSymbols = -1;

    readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);
    if( readSymbols == STATUS_FAIL){
        perror("There are problems while filling table, exactly with reading file");
        free(inputHolder);
        return STATUS_FAIL;
    }

    while (readSymbols > 0){
        while(indexInInputHolder < readSymbols){
            currentStringLength++;
            if(inputHolder[indexInInputHolder] == '\n'){

                offsetsFileTable[indexInTable] = indexInInputHolder + 1 - currentStringLength;
                stringsLengthsFileTable[indexInTable++] = currentStringLength;

                currentStringLength = 0;
            }
            indexInInputHolder++;
        }
        readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);
        if( readSymbols == STATUS_FAIL){
            perror("There are problems while filling table, exactly with reading file");
            free(inputHolder);
            return STATUS_FAIL;
        }
    }

    free(inputHolder);

    return indexInTable;
}

int printStringByNumber(int fileDescriptorIn, long* offsetFileTable, const long* stringsLengthsFileTable, int stringsCount){
    long currentBufferSize = INPUT_HOLDER_SIZE;
    long stopNumber = 0;
    int readSymbols = -1;
    long stringNumber = 1;
    int status = 0;
    char *stringHolder = NULL;

    stringNumber = getStringNumber(stringsCount);
    if(stringNumber == -1){
        return STATUS_FAIL;
    }

    while(stringNumber != stopNumber){

        if(stringNumber < 0 || stringNumber > stringsCount){
            if(stringNumber == -1){
                return STATUS_FAIL;
            }
            fprintf(stderr, "Invalid string number!\n");
            int fflushStatus = fflush(stdout);
            if(fflushStatus != STATUS_SUCCESS){
                perror("There are problems while printing string by number, exactly with fflush command");
                return STATUS_FAIL;
            }
            stringNumber = getStringNumber(stringsCount);
            continue;
        }

        status = lseek(fileDescriptorIn, offsetFileTable[stringNumber-1], SEEK_SET);
        if(status == STATUS_FAIL){
            perror("There are problems while printing string by number, exactly with setting position in file");
            // EBADF, ESPIPE, EINVAL, EOVERFLOW, ENXIO - Ошибки lseek
            return STATUS_FAIL;
        }

        currentBufferSize = stringsLengthsFileTable[stringNumber-1];
        stringHolder = (char*) malloc(currentBufferSize);
        if(stringHolder == NULL){
            perror("There are problems while printing string by number, exactly with allocating memory with malloc");
            free(stringHolder);
            return STATUS_FAIL;
        }

        readSymbols = read(fileDescriptorIn, stringHolder, stringsLengthsFileTable[stringNumber-1]-1);

        if(readSymbols == STATUS_FAIL){
            perror("String number is invalid");
            free(stringHolder);
            return STATUS_FAIL;
        }

        printf("%s\n", stringHolder);
        free(stringHolder);

        stringNumber = getStringNumber(stringsCount);
    }

    printf("Stop number!\n");

    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;

    long offsetsFileTable[256];
    long stringsLengthsFileTable[256];
    int status;

    if(argc < 2){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file");
        exit(EXIT_FAILURE);
    }

    if( (fileDescriptorIn = open(argv[1], O_RDONLY, OWNER_READ_WRITE)) == STATUS_FAIL){
        perror("There are problems while reading file.");
        exit(EXIT_FAILURE);
    }

    int stringsCount = fillTable(offsetsFileTable, stringsLengthsFileTable, fileDescriptorIn);

    if(stringsCount == STATUS_FAIL){
        return EXIT_FAILURE;
    }

    status = printStringByNumber(fileDescriptorIn, offsetsFileTable, stringsLengthsFileTable, stringsCount);

    if(status == STATUS_FAIL) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
