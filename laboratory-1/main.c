#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PATH_MAX 1000

/*
 * Напишите программу, которая будет обрабатывать опции, приведенные ниже. Опции должны быть обработаны в соответствии с порядком своего появления справа налево.
 * Одной и той же опции разрешено появляться несколько раз. Используйте getopt(3C) для определения имеющихся опций.
 * Сначала пусть ваша программа обрабатывает только некоторые опции. Затем добавьте еще, до тех пор, пока все требуемые опции не будут обрабатываться.
 * Вы можете скопировать воспользоваться программой getopt_ex.c и изменить ее.
 *
 *  Done -i  Печатает реальные и эффективные идентификаторы пользователя и группы.
 *   -s  Процесс становится лидером группы. Подсказка: смотри setpgid(2).
 *   -p  Печатает идентификаторы процесса, процесса-родителя и группы процессов.
 *   -u  Печатает значение ulimit
 *   -Unew_ulimit  Изменяет значение ulimit. Подсказка: смотри atol(3C) на странице руководства strtol(3C)
 *   -c  Печатает размер в байтах core-файла, который может быть создан.
 *   -Csize  Изменяет размер core-файла
 *   -d  Печатает текущую рабочую директорию
 *   -v  Распечатывает переменные среды и их значения
 *   -Vname=value  Вносит новую переменную в среду или изменяет значение существующей переменной.
 *
 * Проверьте вашу программу на различных списках аргументов, в том числе:
 *
 *   Нет аргументов
 *   Недопустимую опцию.
 *   Опции, разделенные знаком минус.
 *   Неудачное значение для U.
 *
 */

int main(int argc, char* argv[]){
    char options[] = "ispuU:cC:dvV:";
    int currentArg;
    currentArg = getopt(argc, argv, options);

    char* pathHolder = (char*) malloc(PATH_MAX);
    if(pathHolder == NULL){
        perror("There are problems with allocating memory by malloc.");
    }

    if(argc == 1){
        //fprintf(stderr, "")
        return 0;
    }

    while (currentArg !=  EOF){
        switch (currentArg) {
            case 'i':
                printf("Real user id: %d\n", getuid());
                printf("Effective user id: %d\n", geteuid());
                printf("Real group id: %d\n", getgid());
                printf("Effective group id: %d\n", getegid());
                break;
            case 's':
                break;
            case 'p':
                break;
            case 'u':
                break;
            case 'U':
                break;
            case 'c':
                break;
            case 'C':
                break;
            case 'd':
                if (getcwd(pathHolder, sizeof(pathHolder)) != NULL) {
                    printf("Current Working Directory: %s\n", pathHolder);
                } else {
                    perror("There are problems with getcwd(). ");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                break;
            case 'V':
                break;
        }
        currentArg = getopt(argc, argv, options);
    }

    return 0;
}