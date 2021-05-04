#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define STATUS_FAIL_LSEEK (off_t)-1
#define STATUS_TIMEOUT -2
#define FDS_NOT_READY 0
#define STATUS_NO_NUMCONV -3
#define MAX_STRING_LENGTH 1000
#define OWNER_READ_WRITE 0600
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0
#define STDIN 0
#define STDOUT 1
#define STOPNUMBER 0
#define MAX_FILEDESC_NUMBER 1
#define NOTSTOPNUMBER 1
#define DECIMAL_SCALE_OF_NOTATION 10
#define TABLE_SIZE 4096
#define FD_NOT_INCLUDED 0
#define NO_STRINGS 0

// ! На семинаре я продемонстрировал, что программа работает на Solaris ! ///

extern int errno;

int lastWorkWithData(int fileDescriptorIn){
    int closeStatus = close(fileDescriptorIn);
    if(closeStatus == STATUS_FAIL){
        perror("There are also problems with closing file");
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

// Функция, печающая весь файл (нововведение)
int printAllFile(int fileDescriptorIn){
    char stringHolder[MAX_STRING_LENGTH];
    int readSymbols;
    int writeSymbols;

    int status = lseek(fileDescriptorIn, 0, SEEK_SET);
    if(status == STATUS_FAIL_LSEEK) {
        perror("printAllFile. There are problems while printing string by number, exactly with setting position in file");
        return STATUS_FAIL;
    }

    do {
        readSymbols = read(fileDescriptorIn, stringHolder, MAX_STRING_LENGTH);

        if(readSymbols == STATUS_FAIL){
            perror("printAllFile. Problems with reading file");
            return STATUS_FAIL;
        }
        writeSymbols = write(STDOUT, stringHolder, readSymbols);

        if(writeSymbols == STATUS_FAIL){
            perror("printAllFile. Problems with writing data in file");
            return STATUS_FAIL;
        }

    } while(readSymbols != 0);
    printf("\n");

    return STATUS_SUCCESS;
}

long getStringNumber(int fileDescriptorIn){
    fd_set rfds;
    struct timeval tv;
    int selectStatus;

    // Сбрасываем биты для множества файловых дескрипторов в наборе fd_set. (нововведение)
    FD_ZERO(&rfds);

    // Устанавливаем бит для стандартного потока ввода в наборе fd_set (нововведение)
    FD_SET(STDIN, &rfds);

    // Устанавливаем допустимое время ожидания изменения статуса наборов дескрипторов (нововведение)
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = TIMEOUT_USEC;

    char *endptr = NULL;

    /// Исправление по комментарию с семинара. (#)
    printf("Enter number of line, which you want to see (You have 5 seconds): "); // Иправил, сейчас выводится 1 строка, без отладочной информации

    int fflushStatus = fflush(stdout);
    if(fflushStatus != STATUS_SUCCESS){
        perror("getStringNumber. There are problems while getting your number, exactly with fflush command");
        return STATUS_FAIL;
    }

    // Используем функцию select, для того, чтобы отслеживать статус указанных дескрипторов для чтения
    selectStatus = select(MAX_FILEDESC_NUMBER, &rfds, NULL, NULL, &tv);

    /// Какова семантика нуля? Дайте название ему. /// Исправил. (!)
    if(selectStatus == FDS_NOT_READY){
        fprintf(stderr, "getStringNumber. Time is over!\n");
        printAllFile(fileDescriptorIn);
        return STATUS_TIMEOUT;
    }

    if(selectStatus == STATUS_FAIL){
        perror("getStringNumber. There are problems with select function");
        return STATUS_FAIL;
    }

    char numberHolder[INPUT_HOLDER_SIZE];

    /// Здесь на самом деле вы не знаете с ошибкой завершился select или нет - не проверяете /// Проверил. (!)
    int readSymbols = read(STDIN, numberHolder, INPUT_HOLDER_SIZE);
    if(readSymbols == STATUS_FAIL){
        perror("getStringNumber. There are problems while getting your number, exactly with reading file");
        return STATUS_FAIL;
    }

    /// 10 - magic number. Дайте название /// Исправил. (!)
    long stringNumber = strtol(numberHolder, &endptr, DECIMAL_SCALE_OF_NOTATION);
    if( (stringNumber == LLONG_MAX || stringNumber == LLONG_MIN) && errno == ERANGE){
        perror("getStringNumber. There are problems while getting your number, exactly with converting string to long");
        return STATUS_FAIL;
    }

    if(numberHolder == endptr || *endptr != '\n'){
        return STATUS_NO_NUMCONV;
    }

    return stringNumber;
}

int fillTable(long* offsetsFileTable, long* stringsLengthsFileTable, int fileDescriptorIn){

    char inputHolder[INPUT_HOLDER_SIZE];

    size_t indexInInputHolder = 0;
    size_t currentStringLength = 0;
    size_t indexInTable = 0;
    int currentPosition = 0;
    int readSymbols;

    int terminalNullNumbers = 0;

    /// Для каких целей у вас дублируется код с read? Переработать код так, чтобы вам не приходилось дублировать код.
    /// Исправил (!)

    do {
        readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);

        if( readSymbols == STATUS_FAIL){
            perror("fillTable. There are problems while filling table, exactly with reading file");
            return STATUS_FAIL;
        }

        while(indexInInputHolder < readSymbols || ( readSymbols < INPUT_HOLDER_SIZE && inputHolder[readSymbols] == '\0' ) ){
            currentStringLength++;
            if(inputHolder[indexInInputHolder] == '\n' || inputHolder[indexInInputHolder] == '\0'){

                /// Увеличил размеры таблицы теперь TABLE_SIZE не 256, а 4096. (!)
                /// Если количество строк больше TABLE_SIZE, то программа завершается с указанием на то, что максиммальный размер таблицы превышен.
                if(indexInTable >= TABLE_SIZE){
                    /// Исправление по комментарию с семинара. (#)
                    fprintf(stderr, "fillTable. Strings count is bigger than max table size %d", TABLE_SIZE); /// Исправил, теперь при выводе указывается текущий размер таблицы, объявленный в define
                    return STATUS_FAIL;
                }

                /// Что если ваш файл окажется длиннее 256 строк?
                /// см. выше
                offsetsFileTable[indexInTable] = currentPosition + 1 - currentStringLength;

                /// Что если строка в файле окажется длиннее 256?
                /// Её длина запишится в currentStringLength
                stringsLengthsFileTable[indexInTable] = currentStringLength;

                indexInTable += 1;

                currentStringLength = 0;

                if( inputHolder[indexInInputHolder] == '\0' ){
                    break;
                }
            }
            indexInInputHolder++;
            currentPosition++;
        }

        indexInInputHolder = 0;

        memset(inputHolder, 0, INPUT_HOLDER_SIZE);
    } while (readSymbols > 0);

    return indexInTable;
}

int printStringByNumber(int fileDescriptorIn, long* offsetFileTable, const long* stringsLengthsFileTable, int stringsCount) {
    long currentBufferSize = INPUT_HOLDER_SIZE;
    int readSymbols;
    int lseekStatus;
    long stringNumber;
    int writeStatus;

    do {
        // Получаем число строки, которую хочет видеть пользователь
        stringNumber = getStringNumber(fileDescriptorIn);

        if (stringNumber < 0 || stringNumber > stringsCount) {
            if (stringNumber == STATUS_FAIL) {
                return STATUS_FAIL;
            }

            if (stringNumber == STATUS_TIMEOUT) {
                // Если время вышло, то завершаем печать строк по их номеру
                return STATUS_SUCCESS;
            }

            /// Исправление по комментарию с семинара. (#)
            fprintf(stderr, "printStringByNumber. Invalid string number! Available range: [1, %d]. Try again.\n", stringsCount); /// Добавил вывод допустимого интервала, чтобы пользователь понимал, какой интервал строк ему доступен.

            continue;
        }

        if (stringNumber == 0) {
            break;
        }

        lseekStatus = lseek(fileDescriptorIn, offsetFileTable[stringNumber - 1], SEEK_SET);
        if (lseekStatus == STATUS_FAIL_LSEEK) {
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

        writeStatus = write(STDOUT, stringHolder, currentBufferSize - 1);
        if(writeStatus == STATUS_FAIL){
            perror("printStringByNumber. There are problems with writing data");
            return STATUS_FAIL;
        }
        printf("\n");

    } while (NOTSTOPNUMBER);

    printf("Stop number!\n");
    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){

    int fileDescriptorIn;

    /// 256 - "magic number", дайте название, вынесите в defines (!)
    long offsetsFileTable[TABLE_SIZE];
    long stringsLengthsFileTable[TABLE_SIZE];
    int status;

    if(argc < 2){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    fileDescriptorIn = open(argv[1], O_RDONLY, OWNER_READ_WRITE);
    if(fileDescriptorIn == STATUS_FAIL){
        perror("There are problems while reading file");
        exit(EXIT_FAILURE);
    }

    int stringsCount = fillTable(offsetsFileTable, stringsLengthsFileTable, fileDescriptorIn);
    if(stringsCount == STATUS_FAIL){
        lastWorkWithData(fileDescriptorIn);
        return EXIT_FAILURE;
    }


    if(stringsCount == NO_STRINGS){
        printf("There is no strings in the file.\n");
        lastWorkWithData(fileDescriptorIn);
        return EXIT_SUCCESS;
    }

    printf("Available range: [1, %d]\n", stringsCount);

    status = printStringByNumber(fileDescriptorIn, offsetsFileTable, stringsLengthsFileTable, stringsCount);
    if(status == STATUS_FAIL) {
        lastWorkWithData(fileDescriptorIn);
        return EXIT_FAILURE;
    }

    status = lastWorkWithData(fileDescriptorIn);
    if(status == STATUS_FAIL){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}