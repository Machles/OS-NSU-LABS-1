#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define STATUS_TIMEOUT -2
#define STATUS_NO_NUMCONV -3
#define MAX_STRING_LENGTH 1000
#define OWNER_READ_WRITE 0600
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0
#define STDIN 0
#define STDOUT 1
#define MAX_FILEDESC_NUMBER 1

extern int errno;

int printAllFile(int fileDescriptorIn){
    char stringHolder[MAX_STRING_LENGTH];
    int readSymbols;
    int writeSymbols;

    int status = lseek(fileDescriptorIn, 0, SEEK_SET);
    if(status == STATUS_FAIL){
        perror("printAllFile. There are problems while printing string by number, exactly with setting position in file");
        return STATUS_FAIL;
    }

    printf("\n");
    do {
        readSymbols = read(fileDescriptorIn, stringHolder, MAX_STRING_LENGTH);
        if(readSymbols == STATUS_FAIL){
            perror("printAllFile. Problems with reading file.");
            return STATUS_FAIL;
        }
        writeSymbols = write(STDOUT, stringHolder, readSymbols);
        if(writeSymbols == STATUS_FAIL){
            perror("printAllFile. Problems with writing data in file.");
            return STATUS_FAIL;
        }
    } while(readSymbols != 0);
    printf("\n");

    return STATUS_SUCCESS;
}

long getStringNumber(int stringsCount, int fileDescriptorIn){
    fd_set rfds;
    struct timeval tv;
    int selectStatus;

    FD_ZERO(&rfds);
    FD_SET(STDIN, &rfds);

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = TIMEOUT_USEC;

    char *endptr = NULL;

    printf("There are %d strings.\nEnter number of line, which you want to see (You have 5 seconds): ", stringsCount);
    int status = fflush(stdout);
    if(status != STATUS_SUCCESS){
        perror("getStringNumber. TThere are problems while getting your number, exactly with fflush command");
        return STATUS_FAIL;
    }

    selectStatus = select(MAX_FILEDESC_NUMBER, &rfds, NULL, NULL, &tv);
    if(selectStatus == 0){
        fprintf(stderr, "getStringNumber. TTime is over! Try again.\n");
        printAllFile(fileDescriptorIn);
        return STATUS_TIMEOUT;
    }

    char numberHolder[INPUT_HOLDER_SIZE];

    int readSymbols = read(STDIN, numberHolder, INPUT_HOLDER_SIZE);
    if(readSymbols == STATUS_FAIL){
        perror("getStringNumber. There are problems while getting your number, exactly with reading file");
        return STATUS_FAIL;
    }

    long stringNumber = strtol(numberHolder, &endptr, 10);
    if(stringNumber == -1){
        fprintf(stderr, "getStringNumber. There are problems while getting your number, exactly with converting string to long.\n");
        return STATUS_FAIL;
    }

    if(numberHolder == endptr){
        return STATUS_NO_NUMCONV;
    }

    return stringNumber;
}

int fillTable(long* offsetsFileTable, long* stringsLengthsFileTable, int fileDescriptorIn){

    char inputHolder[INPUT_HOLDER_SIZE];

    size_t indexInInputHolder = 0;
    size_t currentStringLength = 0;
    size_t indexInTable = 0;

    int readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);

    if( readSymbols == STATUS_FAIL){
        perror("fillTable. There are problems while filling table, exactly with reading file");
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
            perror("fillTable. There are problems while filling table, exactly with reading file");
            return STATUS_FAIL;
        }
    }

    return indexInTable;
}

int printStringByNumber(int fileDescriptorIn, long* offsetFileTable, const long* stringsLengthsFileTable, int stringsCount) {
    long currentBufferSize = INPUT_HOLDER_SIZE;
    long stopNumber = 0;
    int readSymbols;
    int status;
    long stringNumber;

    do {
        stringNumber = getStringNumber(stringsCount, fileDescriptorIn);

        if (stringNumber < 0 || stringNumber > stringsCount) {
            if (stringNumber == STATUS_FAIL) {
                return STATUS_FAIL;
            } else if (stringNumber == STATUS_TIMEOUT) {
                continue;
            }
            fprintf(stderr, "printStringByNumber. Invalid string number! Try again.\n");
            continue;
        }

        if (stringNumber == 0) {
            break;
        }

        status = lseek(fileDescriptorIn, offsetFileTable[stringNumber - 1], SEEK_SET);
        if (status == STATUS_FAIL) {
            perror("printStringByNumber. There are problems while printing string by number, exactly with setting position in file");
            return STATUS_FAIL;
        }

        currentBufferSize = stringsLengthsFileTable[stringNumber - 1];
        char stringHolder[currentBufferSize - 1]; // without \n

        readSymbols = read(fileDescriptorIn, stringHolder, currentBufferSize - 1);
        if (readSymbols == STATUS_FAIL) {
            perror("printStringByNumber. String number is invalid");
            return STATUS_FAIL;
        }

        write(STDOUT, stringHolder, currentBufferSize - 1);
        printf("\n");

    } while (stringNumber != stopNumber);

    printf("Stop number!\n");
    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;

    long offsetsFileTable[256];
    long stringsLengthsFileTable[256];
    int status;

    if(argc < 2){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    fileDescriptorIn = open(argv[1], O_RDONLY, OWNER_READ_WRITE);
    if(fileDescriptorIn == STATUS_FAIL){
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