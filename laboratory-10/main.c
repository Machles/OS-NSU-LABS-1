#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define FORK_ERROR (pid_t)-1
#define WAIT_ERROR -1
#define MIN_ARGS_NUM 2
#define CHILD_RETURN_CODE 0
#define PROG_NAME_IDX 1
#define PROG_ARGS_START_IDX 1
#define SLEEP_TIME 1

int executeProgramm(char* argv[], char* programName){
    pid_t statusFork = fork();

    if(statusFork == FORK_ERROR){
        perror("executeProgramm. There are problems with fork");
        return STATUS_FAIL;
    }

    if(statusFork == CHILD_RETURN_CODE){
        int execStatus = execvp(programName, argv);
        if(execStatus == STATUS_FAIL){
            perror("executeProgram. There are problems with execpv");
            return STATUS_FAIL;
        }
    } else {
        sleep(SLEEP_TIME);
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

    if(WIFSIGNALED(currentStatus)){
        int signalInfo = WTERMSIG(currentStatus);
        printf("\nChild process terminated with a signal: %d\n", signalInfo);
    } else if(WIFEXITED(currentStatus)){
        int exitStatus = WEXITSTATUS(currentStatus);
        printf("\nChild process exited with status: %d\n", exitStatus);
    }

    return STATUS_SUCCESS;
}

int main(int argc, char **argv){

    if (argc < MIN_ARGS_NUM) {
        fprintf(stderr, "Not enough arguments entered.\nusage: progname <arg1> <arg2> ... <argN>\n");
        exit(EXIT_FAILURE);
    }

    char* programName = argv[PROG_NAME_IDX];
    char** programArguments = &argv[PROG_ARGS_START_IDX];

    int returnStatus = executeProgramm(programArguments, programName);
    if(returnStatus == STATUS_FAIL){
        fprintf(stderr,"There problems with executing program 'progName'");
        exit(EXIT_FAILURE);
    }

//    printf("Check text.");

    returnStatus = waitForChildProcess();
    if(returnStatus == STATUS_FAIL){
        fprintf(stderr,"There problems with waiting child process");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}