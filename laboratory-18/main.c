#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <libgen.h>
#include <time.h>
#include <string.h>

#define MIN_REQUIRED_ARGS_NUM 2
#define FLAGS_COUNT 3
#define PERMISSION_BITS_COUNT 9
#define TRUE 1
#define STAT_ERROR 1
#define NOT_INCLUDED 0

#define LAST_STRING_CHARACTER '\0'

void printFilename(char* pathFile){
    char * fileName = basename(pathFile);
    printf("%s\t", fileName);
}

void printLastTimeFileModified(struct stat* statbuf){
    time_t time = statbuf->st_mtim.tv_sec;
    char * lastTimeFileModified = ctime(&time);

    unsigned long timeLength = strlen(lastTimeFileModified);
    lastTimeFileModified[timeLength-1] = LAST_STRING_CHARACTER;

    printf("%s\t", lastTimeFileModified);
}

void printRegularFileSize(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;
    if(S_ISREG(mode)){
        off_t fileSize = statbuf->st_size;
        printf("%ld\t", fileSize);
    } else {
        printf("\t");
    }
}

void printOwners(struct stat* statbuf){
    uid_t uid = statbuf->st_uid;
    struct passwd * pw = getpwuid(uid);

    gid_t gid = statbuf->st_gid;
    struct group * grp = getgrgid(gid);

    if(pw != NULL){
        printf("%s\t", pw->pw_name);
    } else {
        printf("%d\t", uid);
    }

    if(grp != NULL){
        printf("%s\t ", grp->gr_name);
    } else {
        printf("%d\t ", gid);
    }
}

void printHardLinksNumber(struct stat* statbuf){
    nlink_t hLinksNumber = statbuf->st_nlink;
    printf("%lu\t ", hLinksNumber);
}

void printFilePermissions(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;

    const char flags[FLAGS_COUNT] = {'r', 'w', 'x'};
    char permissions[] = "----------";
    const mode_t permissionBits[PERMISSION_BITS_COUNT] = { S_IRUSR, S_IWUSR, S_IXUSR,
                                                           S_IRGRP, S_IWGRP, S_IXGRP,
                                                           S_IROTH, S_IWOTH, S_IXOTH };

    for (int i = 0; i < PERMISSION_BITS_COUNT; ++i) {
        if( (mode & permissionBits[i]) != NOT_INCLUDED){
            permissions[i] = flags[i % FLAGS_COUNT];
        }
    }

    permissions[PERMISSION_BITS_COUNT] = LAST_STRING_CHARACTER;

    printf("%s\t", permissions);
}

void printFileType(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;
    if(S_ISDIR(mode)){
        printf("%c", 'd');
    } else if(S_ISREG(mode)){
        printf("%c", '-');
    } else {
        printf("%c", '?');
    }
}

void printFileInformation(struct stat* statbuf, char * pathFile){
    printFileType(statbuf);
    printFilePermissions(statbuf);
    printHardLinksNumber(statbuf);
    printOwners(statbuf);
    printRegularFileSize(statbuf);
    printLastTimeFileModified(statbuf);
    printFilename(pathFile);
    printf("\n");
}

int main(int argc, char **argv){

    struct stat statbuf;
    char * pathFile;
    int statStatus;

    char * argvDef[] = {argv[0] ,".", NULL};
    int argcDef = 2;

    if(argc < MIN_REQUIRED_ARGS_NUM){
        argv = argvDef;
        argc = argcDef;
    }

    for (int i = 1; i < argc; ++i) {
        pathFile = argv[i];
        statStatus = stat(pathFile, &statbuf);

        if(statStatus == STAT_ERROR){
            perror("There are problems with stat");
            continue;
        }

        printFileInformation(&statbuf, pathFile);
    }

    return EXIT_SUCCESS;
}

