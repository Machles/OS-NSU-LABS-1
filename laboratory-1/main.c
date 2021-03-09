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

int printRealAndEffectiveUIDAndGID(){
    printf("Real user id: %d\n", getuid());
    printf("Effective user id: %d\n", geteuid());
    printf("Real group id: %d\n", getgid());
    printf("Effective group id: %d\n", getegid());

    return STATUS_SUCCESS;
}

int makeProcessGroupLeader(){
    int status = setpgid(0, 0);
    if(status != STATUS_SUCCESS){
        perror("There are problems with process's becoming leader of the group");
        return STATUS_FAIL;
    }
    printf("Successfully process became leader of the group.");

    return STATUS_SUCCESS;
}

int printPIDandPPIDandPGID(){
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    printf("Group of Processes ID: %d\n", getpgid(0) );

    return STATUS_SUCCESS;
}

int printUlimitValue(){
    long ulimitValue = ulimit(UL_GETFSIZE);

    if(ulimitValue == STATUS_FAIL){
        perror("There are problems with getting ulimit");
        return STATUS_FAIL;
    }

    printf("Current max file size for process is: %lu\n", ulimitValue);

    return STATUS_SUCCESS;
}

int changeUlimitValue(){
    char* UValuePointer = optarg;
    char* endptr = NULL;

    long stringNumber = strtol(UValuePointer, &endptr, 10);
    if(stringNumber < 0){
        fprintf(stderr, "Invalid ulimit value");
        return STATUS_FAIL;
    }

    long status = ulimit(UL_SETFSIZE, stringNumber);
    if(status == STATUS_FAIL){
        perror("There are problems with setting new ulimit value");
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

int printCoreFileSize(){
    struct rlimit rlp;
    int coreFileSize = getrlimit(RLIMIT_CORE, &rlp);
    if(coreFileSize == STATUS_FAIL){
        perror("There are problems with getting core-file size");
        return STATUS_FAIL;
    }
    printf("Core Size is: %lu", rlp.rlim_cur );

    return STATUS_SUCCESS;
}

int changeCoreFileSize(){
    struct rlimit rlp;
    char* CValuePointer = optarg;
    char* endptr = NULL;

    getrlimit(RLIMIT_CORE, &rlp);

    long stringNumber = strtol(CValuePointer, &endptr, 10);
    if(stringNumber < 0){
        fprintf(stderr, "Invalid core-file size");
        return STATUS_FAIL;
    }

    rlp.rlim_cur = stringNumber;

    if (setrlimit(RLIMIT_CORE, &rlp) == STATUS_FAIL) {
        perror("There are some problems with setting new value of ulimit");
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}

int printCurrentDirectory(){
    char* pathHolder = getcwd(NULL, PATH_MAX);
    if (pathHolder == NULL) {
        perror("There are problems with getcwd().");
        return STATUS_FAIL;
    }

    printf("Current Working Directory: %s\n", pathHolder);

    return STATUS_SUCCESS;
}

int printEnvironmentVariables(){
    printf("Environmental Variables: \n");
    for(char** envVariable = environ; *envVariable; ++envVariable){
        printf("%s\n", *envVariable);
    }

    return STATUS_SUCCESS;
}

int changeOrAddEnvironmentVariable(){
    char* VValuePointer = optarg;
    int status = putenv(VValuePointer);
    if(status != STATUS_SUCCESS){
        perror("There are problems with putting new environmental variable");
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}

int main(int argc, char* argv[]){
    char options[] = "ispuU:cC:dvV:";
    int currentArg = getopt(argc, argv, options);
    int status = STATUS_FAIL;

    if(argc == 1){
        printf( "No options here.\n");
        return 0;
    }

    while (currentArg !=  EOF){
        switch (currentArg) {
            case 'i':
                printRealAndEffectiveUIDAndGID();
                break;
            case 's':
                status = makeProcessGroupLeader();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'p':
                printPIDandPPIDandPGID(); break;
            case 'u':
                status = printUlimitValue();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'U':
                status = changeUlimitValue();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                status = printCoreFileSize();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'C':
                status = changeCoreFileSize();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'd':
                status = printCurrentDirectory();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                printEnvironmentVariables(); break;
            case 'V':
                status = changeOrAddEnvironmentVariable();
                if(status == STATUS_FAIL){
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                fprintf(stderr, "Option isn't correct!\n");
                exit(EXIT_FAILURE);
        }
        currentArg = getopt(argc, argv, options);
    }

    return EXIT_SUCCESS;
}