#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define STATUS_SUCCESS 0
#define STATUS_FAIL -1
#define FORK_ERROR -1
#define WAIT_ERROR -1
#define REQUIRED_ARGS_NUM 2
#define CHILD_RETURN_CODE 0

int executeProgramm(char* argv[], char* programName){
    pid_t statusFork = fork();
    if(statusFork == FORK_ERROR){
        perror("executeProgramm. There are problems with fork");
        return STATUS_FAIL;
    }
    if(statusFork == CHILD_RETURN_CODE){
        int execStatus = execvp(programName, argv);
        if(execStatus != STATUS_FAIL){
            perror("executeProgram. There are problems with execpv");
            return STATUS_FAIL;
        }
    }

    return STATUS_SUCCESS;
}

int waitForChildProcess(){
    int statloc = 0;
    pid_t statusWait = wait(&statloc);
    if(statusWait == WAIT_ERROR){
        perror("waitForChildProcess. There are problems with wait");
        return STATUS_FAIL;
    }

    if(WIFSIGNALED(statusWait)){
        int exitStatus = WEXITSTATUS(statusWait);
        printf("\nChild process exited with status: %d", statusWait);
    } else if(WIFEXITED(statusWait)){
        int signalInfo = WTERMSIG(statusWait);
        printf("\nChild process terminated with a signal: %d", signalInfo);
    }

    return STATUS_SUCCESS;
}

int main(int argc, char **argv){

    if (argc != REQUIRED_ARGS_NUM) {
        fprintf(stderr, "Not enough arguments entered.\nusage: progname input_file\n");
        exit(EXIT_FAILURE);
    }

    char programName[] = "cat";

    char* programPath = argv[0];
    char* filename = argv[1];
    char* commandArgv[] = {programName, filename, NULL};

    int returnStatus = executeProgramm(argv, programName);
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