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

#define HLINK_FIELD_WIDTH 4
#define OWNERS_FIELD_WIDTH 10
#define FILESIZE_FIELD_WIDTH 10
#define FILENAME_FIELD_WIDTH 10

#define LAST_STRING_CHARACTER '\0'

void printFilename(char* pathFile){
    char * fileName = basename(pathFile);
    printf("%*s", FILENAME_FIELD_WIDTH, fileName);
}

void printLastTimeFileModified(struct stat* statbuf){
    time_t time = statbuf->st_mtim.tv_sec;
    char *lastTimeFileModified = ctime(&time);

    unsigned long timeLength = strlen(lastTimeFileModified);
    lastTimeFileModified[timeLength-1] = LAST_STRING_CHARACTER;

    printf("%s ", lastTimeFileModified);
}

void printRegularFileSize(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;
    if(S_ISREG(mode)){
        off_t fileSize = statbuf->st_size;
        printf("%*ld ", FILESIZE_FIELD_WIDTH, fileSize);
    } else {
        printf("%*s ", FILESIZE_FIELD_WIDTH, " ");
    }
}

void printOwners(struct stat* statbuf){
    uid_t uid = statbuf->st_uid;
    struct passwd * pw = getpwuid(uid);

    gid_t gid = statbuf->st_gid;
    struct group * grp = getgrgid(gid);

    if(pw != NULL){
        printf("%*s ", OWNERS_FIELD_WIDTH, pw->pw_name);
    } else {
        printf("%*d ", OWNERS_FIELD_WIDTH, uid);
    }

    if(grp != NULL){
        printf("%*s ", OWNERS_FIELD_WIDTH, grp->gr_name);
    } else {
        printf("%*d ", OWNERS_FIELD_WIDTH, gid);
    }
}

void printHardLinksNumber(struct stat* statbuf){
    nlink_t hLinksNumber = statbuf->st_nlink;
    printf("%*lu ", HLINK_FIELD_WIDTH, hLinksNumber);
}

void printFilePermissions(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;

    const char flags[FLAGS_COUNT] = {'r', 'w', 'x'};
    char permissions[PERMISSION_BITS_COUNT] = "---------";
    const mode_t permissionBits[PERMISSION_BITS_COUNT] = { S_IRUSR, S_IWUSR, S_IXUSR,
                                                           S_IRGRP, S_IWGRP, S_IXGRP,
                                                           S_IROTH, S_IWOTH, S_IXOTH };

    for (int i = 0; i < PERMISSION_BITS_COUNT; ++i) {
        if( (mode & permissionBits[i]) != NOT_INCLUDED){
            permissions[i] = flags[i % FLAGS_COUNT];
        }
    }

    printf("%s ", permissions);
}

void printFileType(struct stat* statbuf){
    mode_t mode = statbuf->st_mode;
    if(S_ISDIR(mode)){
        printf("%c", 'd');
    } if(S_ISREG(mode)){
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

    if(argc < MIN_REQUIRED_ARGS_NUM){
        fprintf(stderr, "Not enough arguments entered.\nusage: progname <file1> <file2> ... <fileN>\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i) {
        pathFile = argv[i];

        statStatus = stat(pathFile, &statbuf);
        if(statStatus == STAT_ERROR){
            perror("There are problems with stat.");
            continue;
        }

        printFileInformation(&statbuf, pathFile);
    }

    return EXIT_SUCCESS;
}

