#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN_REQUIRED_ARGS_NUM 2
#define STATUS_FAIL -1
#define PROG_NAME_IDX 1
#define PROG_ARGS_START_IDX 1

extern char ** environ;

int execvpe(const char* filename, char *argv[], char *envp[]){
    char **prevEnviron = environ;
    environ = envp;
    execvp(filename, argv);
    perror("There are problems with execvp");
    environ = prevEnviron;

    return STATUS_FAIL;
}

int main(int argc, char **argv){
    if(argc < MIN_REQUIRED_ARGS_NUM){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname commandName <arg1> <arg2> ... <argN>\n");
        exit(EXIT_FAILURE);
    }

    char *envpMod[2] = {"", NULL};
    char* fileName = argv[PROG_NAME_IDX];
    char** commandArguments = &argv[PROG_ARGS_START_IDX];

    execvpe(fileName, commandArguments, envpMod);

    return EXIT_FAILURE;
}
