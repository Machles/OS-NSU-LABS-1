#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define FORK_ERROR (pid_t)-1
#define WAIT_ERROR -1
#define REQUIRED_ARGS_NUM 2
#define CHILD_RETURN_CODE 0
#define FILENAME_IDX 1
#define SLEEP_TIME 1

int executeCommand(char* argv[], char* commandName){
    pid_t statusFork = fork();

    if(statusFork == FORK_ERROR){
        perror("executeCommand. There are problems with fork");
        return STATUS_FAIL;
    }

    if(statusFork == CHILD_RETURN_CODE){
        execvp(commandName, argv);
        perror("executeCommand. There are problems with execpv");
        return STATUS_FAIL;
    } else {
        sleep(1);
    }

    return STATUS_SUCCESS;
}

int waitForChildProcess(){
    int currentStatus = 0;
    pid_t statusWait = wait(&currentStatus);

    if(statusWait == WAIT_ERROR){
        perror("waitForChildProcess. There are problems with wait");
        return STATUS_FAIL;
    }

    // В этом месте выводится информация родительским процессом о статусе завершения дочернего процесса
    // т.е. в данном случае родительский процесс выводит последнюю свою строку после завершения работы дочернего процесса.
    if(WIFSIGNALED(currentStatus)){
        int signalInfo = WTERMSIG(currentStatus);
        printf("Child process terminated with a signal: %d\n", signalInfo);
        if(WCOREDUMP(currentStatus)){
            printf("Also core file has been produced.");
        }
    } else if(WIFEXITED(currentStatus)){
        int exitStatus = WEXITSTATUS(currentStatus);
        printf("Child process exited with status: %d\n", exitStatus);
    }

    return STATUS_SUCCESS;
}

int main(int argc, char **argv){

    if (argc != REQUIRED_ARGS_NUM) {
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    // Здесь я явно задаю название команды, которую хочу запустить
    char commandName[] = "cat";
    char* fileName = argv[FILENAME_IDX];

    // Создаю на основе предудущих данных новый массив аргументов
    char* commandArgv[] = {commandName, fileName, NULL};

    int returnStatus = executeCommand(commandArgv, commandName);
    if(returnStatus == STATUS_FAIL){
        fprintf(stderr,"There problems with executing command 'commandName'");
        exit(EXIT_FAILURE);
    }

    // Первый вариант программы - "Родитель должен вызвать printf(3) и распечатать какой-либо текст."
    printf("Check text\n");

    // Второй вариант программы - модифицированный - "Последняя строка, распечатанная родителем, выводилась после завершения порожденного процесса."
//    returnStatus = waitForChildProcess();
//    if(returnStatus == STATUS_FAIL){
//        fprintf(stderr,"There problems with waiting child process");
//        exit(EXIT_FAILURE);
//    }

    return EXIT_SUCCESS;
}