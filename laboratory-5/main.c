#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#define INPUT_HOLDER_SIZE 512

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define STATUS_NO_NUMCONV -3
#define MAX_STRING_LENGTH 1000
#define OWNER_READ_WRITE 0600
#define STDIN 0
#define STDOUT 1
#define STOPNUMBER 0
#define NOTSTOPNUMBER 1

extern int errno;

/// Объясните почему нельзя использовать scanf?
/* strtol будет лучше и понятнее себя вести, когда на вход попадет строка, содержащая очень больше число,
 * если число окажется больше LONG_MAX или меньше LONG_MIN - strtol вернёт LONG_MAX или LONG_MIN и установит errno ERANGE.
 * В свою очередь, scanf даже не уведомит о том, что что-то не так
 */
long getStringNumber(int stringsCount){
    char *endptr = NULL;
    printf("There are %d strings.\nEnter number of line, which you want to see: ", stringsCount);
    int status = fflush(stdout);
    if(status != STATUS_SUCCESS){
        perror("getStringNumber. There are problems while getting your number, exactly with fflush command");
        return STATUS_FAIL;
    }

    char numberHolder[INPUT_HOLDER_SIZE];

    int readSymbols = read(STDIN, numberHolder, INPUT_HOLDER_SIZE);

    if(readSymbols == STATUS_FAIL){
        perror("getStringNumber. There are problems while getting your number, exactly with reading file");
        return STATUS_FAIL;
    }

    long stringNumber = strtol(numberHolder, &endptr, 10);
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

/// На самом деле ваша функция ничего не возвращает. Название неподходящее.
/// Поменял
int printStringByNumber(int fileDescriptorIn, long* offsetFileTable, const long* stringsLengthsFileTable, int stringsCount){
    long currentBufferSize = INPUT_HOLDER_SIZE;
    int readSymbols;
    int status;
    long stringNumber;

    /// избегайте слова "flag" в переменных, в большинстве случаев это бессмысленный суффикс
    /// подумайте, каков общий смысл, до каких пор выполняется обработка строк? соответственно дайте название предикату
    /// неявное приведение типов, используйте сравнение с конкретным значением
    /// Флаг заменил на условие достижения стопсимвола.
    do {
        stringNumber = getStringNumber(stringsCount);

        if (stringNumber < 0 || stringNumber > stringsCount) {
            if (stringNumber == STATUS_FAIL) {
                return STATUS_FAIL;
            }
            fprintf(stderr, "printStringByNumber. Invalid string number! Try again.\n");
            continue;
        }

        if (stringNumber == STOPNUMBER) {
            break;
        }

        /// В описании вы указали, что lseek может возвращать ошибки (какие?) - но обработчика ошибок нет
        /// Теперь есть, ниже указаны ошибки.
        status = lseek(fileDescriptorIn, offsetFileTable[stringNumber - 1], SEEK_SET);
        if (status == STATUS_FAIL) {
            /// Какие ошибки может вернуть lseek?
            /// EBADF, ESPIPE, EINVAL, EOVERFLOW, ENXIO - Ошибки lseek
            perror("printStringByNumber. There are problems while printing string by number, exactly with setting position in file");
            return STATUS_FAIL;
        }

        currentBufferSize = stringsLengthsFileTable[stringNumber - 1];
        char stringHolder[currentBufferSize - 1]; // without \n

        readSymbols = read(fileDescriptorIn, stringHolder, currentBufferSize - 1);
        if (readSymbols == STATUS_FAIL) {
            perror("printStringByNumber. String number is invalid");
            ///Это очень плохая практика в любой обработке ошибки завершать процесс - это неожиданное поведение, такой код практически нельзя переиспользовать, он очень нестабилен. Нужно исправить.
            /// В общем случае при ошибке: освобождайте ресурсы (где требуется), возвращайте код ошибки, и далее в основном коде обрабатывайте ошибку
            /// Исправил
            return STATUS_FAIL;
        }

        write(STDOUT, stringHolder, currentBufferSize - 1);
        printf("\n");

    } while (NOTSTOPNUMBER);

    printf("Stop number!\n");

    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){

    int fileDescriptorIn = 0;

    /// Названия переменных плохие. Вы одновременно используете camelCase и "_" - выбере что-то одно. "_T" - не говорит ни чём
    /// Исправил
    long offsetsFileTable[256];
    long stringsLengthsFileTable[256];
    int status;

    if(argc < 2){
        /// Куда происходит вывод сообщение об ошибке сейчас? а куда следует на самом деле выводить сообщение об ошибке?
        /// Вывод сообщения об ошибке сейчас происходит в стандартный поток вывода, а следует выводить в поток вывода ошибок stderr
        /// Опишите пример usage для пользотеля так, как это делается в стандартных утилитах. Иначе как пользователь угадает, что от него хотят?
        /// Описал
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    /// Что такое "0600"? Замените на именованную константу или макрос
    /// Это 8ричное число, которое указывает, что файл читать и писать может только владелец. Заменил на макрос.
    fileDescriptorIn = open(argv[1], O_RDONLY, OWNER_READ_WRITE);
    if( fileDescriptorIn == STATUS_FAIL ){
        perror("There are problems while reading file.");
        exit(EXIT_FAILURE);
    }

    int stringsCount = fillTable(offsetsFileTable, stringsLengthsFileTable, fileDescriptorIn);

    if(stringsCount == STATUS_FAIL){
        /// Количество элементов принято обозначать словом "count". Amount - это скорее "сумма", про валюту
        /// Исправил
        return EXIT_FAILURE;
    }

    status = printStringByNumber(fileDescriptorIn, offsetsFileTable, stringsLengthsFileTable, stringsCount);

    if(status == STATUS_FAIL) {
        return EXIT_FAILURE;
    }

    close(fileDescriptorIn);
    return EXIT_SUCCESS;
}
