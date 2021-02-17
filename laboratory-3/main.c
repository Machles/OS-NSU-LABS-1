#include <stdlib.h>
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>

/*
 * Создайте файл данных, который может писать и читать только владелец (это можно сделать командой shell chmod 600 file) и напишите программу, которая
    Печатает реальный и эффективный идентификаторы пользователя.
    Открывает файл с помощью fopen(3). Если fopen() завершился успешно, файл должен быть закрыт с помощью fclose(3).
    Напечатайте сообщение об ошибке, используя perror(3C), если файл не удалось открыть.
    Сделайте, чтобы реальный и эффективный идентификаторы пользователя совпадали. Подсказка: setuid(2)
    Повторите первые два шага.

Проверьте работу вашей программы.

    Исполните программу и посмотрите вывод
    Сделайте программу доступной для запуска членам вашей группы и пусть ваши одногруппники исполнят программу.
    Командой chmod u+s prog установите бит установки идентификатора пользователя и пусть ваши одногруппники опять исполнят эту программу.
 */

int main(int argc, char* argv[]){
    FILE* file = NULL;

    file = fopen(argv[1], "r");

    printf("Real user ID: %d\nEffective user ID: %d\n\n", getuid(), geteuid());

    if(file == NULL){
        perror("There are problems with opening file. (First try)\n\n");
        exit(EXIT_FAILURE);
    } else {
        printf("File is opened. (First try)\n\n");
        fclose(file);
    }

    setuid(getuid());

    file = fopen(argv[1], "r");

    printf("New real user ID: %d\nNew effective user ID: %d\n\n", getuid(), geteuid());

    if(file == NULL){
        perror("There are problems with opening file. (Second try)\n\n");
        exit(EXIT_FAILURE);
    } else {
        printf("File is opened. (Second try)\n\n");
        fclose(file);
    }

    return EXIT_SUCCESS;
}