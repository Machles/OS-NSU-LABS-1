#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ulimit.h>
#include <sys/resource.h>

#define PATH_MAX 100
#define STATUS_SUCCESS 0
#define STATUS_FAIL -1

extern char ** environ;

void printRealAndEffectiveUIDAndGID(){
    printf("Real user id: %d\n", getuid());
    printf("Effective user id: %d\n", geteuid());
    printf("Real group id: %d\n", getgid());
    printf("Effective group id: %d\n", getegid());
}

void makeProcessGroupLeader(){
    int status = setpgid(0, 0);
    if(status == STATUS_SUCCESS){
        printf("Successfully process became leader of the group.");
    } else {
        perror("There are problems with process's becoming leader of the group");
    }
}

void printPIDandPPIDandPGID(){
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("Group of Processes ID: %d\n", getpgid(0) );
}

void printUlimitValue(){
    long ulimitValue = STATUS_FAIL;

    ulimitValue = ulimit(UL_GETFSIZE);
    if(ulimitValue == STATUS_FAIL){
        perror("There are problems with getting ulimit.\n");
    } else {
        printf("Current max file size for process is: %lu\n", ulimitValue);
    }
}

void changeUlimitValue(){
    char* UValuePointer = optarg;
    char* endptr;

    long stringNumber = strtol(optarg, &endptr, 10);
    if(stringNumber == 0 && endptr != NULL || stringNumber < 0){
        fprintf(stderr, "Invalid ulimit value");
    }

    int status = ulimit(UL_SETFSIZE, atol(optarg));
    if(status == STATUS_FAIL){
        perror("There are problems with setting new ulimit value.\n");
    }

    printf("%s", UValuePointer);
}

void printCoreFileSize(){
    struct rlimit rlp;
    int coreFileSize = getrlimit(RLIMIT_CORE, &rlp);
    if(coreFileSize == STATUS_FAIL){
        perror("There are problems with getting core-file size.");
    }
    printf("Core Size is: %lu", rlp.rlim_cur );
}

void changeCoreFileSize(){
    struct rlimit rlp;
    char* CValuePointer = optarg;

    getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = atol(CValuePointer);

    if (setrlimit(RLIMIT_CORE, &rlp) == STATUS_FAIL)
        perror( "There are some problems with setting new value of ulimit.\n");
}

void printCurrentDirectory(){
    char* pathHolder = getcwd(NULL, PATH_MAX);
    if (pathHolder != NULL) {
        printf("Current Working Directory: %s\n", pathHolder);
    } else {
        perror("There are problems with getcwd().");
        exit(EXIT_FAILURE);
    }
}

void printEnvironmentVariables(){
    printf("Environmental Variables: \n");
    for(char** envVariable = environ; *envVariable; ++envVariable){
        printf("%s\n", *envVariable);
    }
}

void changeOrAddEnvironmentVariable(){
    char* VValuePointer = optarg;
    int status = putenv(VValuePointer);
    if(status != STATUS_SUCCESS){
        perror("There are problems with putting new environmental variable.");
    }
}

int main(int argc, char* argv[]){
    char options[] = "ispuU:cC:dvV:";
    int currentArg = getopt(argc, argv, options);

    if(argc == 1){
        printf( "No options here.\n");
        return 0;
    }

    while (currentArg !=  EOF){
        switch (currentArg) {
            case 'i':
                printRealAndEffectiveUIDAndGID(); break;
            case 's':
                makeProcessGroupLeader(); break;
            case 'p':
                printPIDandPPIDandPGID(); break;
            case 'u':
                printUlimitValue(); break;
            case 'U':
                changeUlimitValue(); break;
            case 'c':
                printCoreFileSize(); break;
            case 'C':
                changeCoreFileSize(); break;
            case 'd':
                printCurrentDirectory(); break;
            case 'v':
                printEnvironmentVariables(); break;
            case 'V':
                changeOrAddEnvironmentVariable(); break;
            case '?':
                fprintf(stderr, "Option isn't correct!\n");
                exit(EXIT_FAILURE);
        }
        currentArg = getopt(argc, argv, options);
    }

    return EXIT_SUCCESS;
}