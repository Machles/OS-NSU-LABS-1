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
/* strtol будет лучше и понятнее себя вести, когда на вход попадет строка, содержащая очень больше число,
 * если число окажется больше LONG_MAX или меньше LONG_MIN - strtol вернёт -1 и установит errno ERANGE.
 * В свою очередь, scanf даже не уведомит о том, что что-то не так
 */

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
    if(stringNumber < 0){
        fprintf(stderr, "There are problems while getting your number, exactly with converting string to long.\n");
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
    int readSymbols = read(fileDescriptorIn, inputHolder, INPUT_HOLDER_SIZE);

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

// На самом деле ваша функция ничего не возвращает. Название неподходящее. - Поменял
int printStringByNumber(int fileDescriptorIn, long* offsetFileTable, const long* stringsLengthsFileTable, int stringsCount){
    long currentBufferSize = INPUT_HOLDER_SIZE;
    long stopNumber = 0;
    int readSymbols = -1;
    int status = 0;
    char *stringHolder = NULL;

    long stringNumber = getStringNumber(stringsCount);
    if(stringNumber == -1){
        return STATUS_FAIL;
    }
    // избегайте слова "flag" в переменных, в большинстве случаев это бессмысленный суффикс
    // подумайте, каков общий смысл, до каких пор выполняется обработка строк? соответственно дайте название предикату
    // неявное приведение типов, используйте сравнение с конкретным значением - Флаг заменил на условие достиженяи стопсимвола.
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

        /// В описании вы указали, что lseek может возвращать ошибки (какие?) - но обработчика ошибок нет - теперь есть, ниже указаны ошибки
        status = lseek(fileDescriptorIn, offsetFileTable[stringNumber-1], SEEK_SET);
        if(status == STATUS_FAIL){
            perror("There are problems while printing string by number, exactly with setting position in file");
            // Какие ошибки может вернуть lseek?
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

    /// Названия переменных плохие. Вы одновременно используете camelCase и "_" - выбере что-то одно. "_T" - не говорит ни чём - исправил
    long offsetsFileTable[256];
    long stringsLengthsFileTable[256];
    int status;

    if(argc < 2){
        /// Куда происходит вывод сообщение об ошибке сейчас? а куда следует на самом деле выводить сообщение об ошибке?
        /// Опишите пример usage для пользотеля так, как это делается в стандартных утилитах. Иначе как пользователь угадает, что от него хотят?
//      /// Описал
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    /// Что такое "0600"? Замените на именованную константу или макрос - Это 8ричное число, которое указывает, что файл читать и писать может только владелец.
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
