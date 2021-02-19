#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ulimit.h>

#define PATH_MAX 100
#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

/*
 * Напишите программу, которая будет обрабатывать опции, приведенные ниже. Опции должны быть обработаны в соответствии с порядком своего появления справа налево.
 * Одной и той же опции разрешено появляться несколько раз. Используйте getopt(3C) для определения имеющихся опций.
 * Сначала пусть ваша программа обрабатывает только некоторые опции. Затем добавьте еще, до тех пор, пока все требуемые опции не будут обрабатываться.
 * Вы можете скопировать воспользоваться программой getopt_ex.c и изменить ее.
 *
 *  Done. -i  Печатает реальные и эффективные идентификаторы пользователя и группы.
 *  Done. -s  Процесс становится лидером группы. Подсказка: смотри setpgid(2).
 *  Done. -p  Печатает идентификаторы процесса, процесса-родителя и группы процессов.
 *  Done. -u  Печатает значение ulimit
 *   -Unew_ulimit  Изменяет значение ulimit. Подсказка: смотри atol(3C) на странице руководства strtol(3C)
 *   -c  Печатает размер в байтах core-файла, который может быть создан.
 *   -Csize  Изменяет размер core-файла
 *  Done. -d  Печатает текущую рабочую директорию
 *  Done. -v  Распечатывает переменные среды и их значения
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
extern char ** environ;

int main(int argc, char* argv[]){
    char options[] = "ispuU:cC:dvV:";
    int currentArg;
    currentArg = getopt(argc, argv, options);

    char* pathHolder = NULL;

    if(argc == 1){
        printf( "No options here.\n");
        return 0;
    }

    int status = STATUS_SUCCESS;
    long ulimitValue = STATUS_FAIL;

    while (currentArg !=  EOF){
        switch (currentArg) {
            case 'i':
                printf("Real user id: %d\n", getuid());
                printf("Effective user id: %d\n", geteuid());
                printf("Real group id: %d\n", getgid());
                printf("Effective group id: %d\n", getegid());
                break;
            case 's':
                status = setpgid(0, 0);
                if(status == STATUS_SUCCESS){
                    printf("Successfully process became leader of the group.");
                } else {
                    perror("There are problems with process's becoming leader of the group.");
                }
                break;
            case 'p':
                printf("Process ID: %d\n", getpid());
                printf("Parent Process ID: %d\n", getppid());
                printf("Group of Processes ID: %d\n", getpgid(0) );
                break;
            case 'u':
                ulimitValue = ulimit(UL_GETFSIZE, 0);
                if(ulimitValue == STATUS_FAIL){
                    perror("There are problems with getting ulimit.\n");
                } else {
                    printf("ulimit is: %lu\n", ulimitValue);
                }
                break;
            case 'U':
                break;
            case 'c':
                break;
            case 'C':
                break;
            case 'd':
                pathHolder = getcwd(NULL, PATH_MAX);
                if (pathHolder != NULL) {
                    printf("Current Working Directory: %s\n", pathHolder);
                } else {
                    perror("There are problems with getcwd(). ");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                printf("Environmental Variables: \n");
                for(char** envVariable = environ; *envVariable; ++envVariable){
                    printf("%s\n", *envVariable);
                }
                break;
            case 'V':
                break;
            default:
                fprintf(stderr, "Option isn't correct!");
                exit(EXIT_FAILURE);
        }
        currentArg = getopt(argc, argv, options);
    }

    return 0;
}