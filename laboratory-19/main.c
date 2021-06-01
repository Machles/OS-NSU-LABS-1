#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#define STATUS_SUCCESS 0
#define STATUS_FAILURE (-1)

#define REALLOC_ERROR NULL
#define FGETS_ERROR NULL

#define NEWLINE_CHARACTER '\n'
#define ENDSTRING_CHARACTER '\0'

#define INPUT_HOLDER_INIT_SIZE 64
#define EXPAND_COEF 2
#define STRING_NOT_FOUND NULL

#define TRUE 1
#define FALSE 0

void correctCompletion(char * pattern){
    free(pattern);
}

int globErrfunc(const char *epath, int eerrno){
    fprintf(stderr,"There are problems with %s, certanly: %s", epath, strerror(eerrno));
    return STATUS_SUCCESS;
}

int checkSlashInPattern(const char * pattern){
    char* slashSybol = "/";

    if(strstr(pattern, slashSybol) != STRING_NOT_FOUND){
        return TRUE;
    }
    return FALSE;
}

int printSuitableStrings(char * pattern){
    glob_t suitableStrings;

    if(checkSlashInPattern(pattern) == TRUE){
        fprintf(stderr, "printSuitableStrings. Incorrect pattern. '/' symbol is prohibited!\n");
        return STATUS_FAILURE;
    }

    // GLOB_NOCHECK
    // If pattern does not match any path name, then glob() returns a list consisting of only pattern, and the number of matched path names is 1.
    int globStatus = glob(pattern, GLOB_NOCHECK, globErrfunc, &suitableStrings);

    if(globStatus != STATUS_SUCCESS){
        perror("printSuitableStrings. There are problems with glob");
        return STATUS_FAILURE;
    }

    char **filenamesList = suitableStrings.gl_pathv;
    int filenamesNumber = suitableStrings.gl_pathc;

    printf("Work's results:\n");
    for (int i = 0; i < filenamesNumber; ++i) {
        printf("%s\n", filenamesList[i]);
    }

    globfree(&suitableStrings);

    return STATUS_SUCCESS;
}

int expandInputBuffer(char** inputHolder, size_t *bufferSize){
    char *newInputHolder = NULL;
    int newInputHolderSize;

    if(*bufferSize == 0){
        newInputHolderSize = INPUT_HOLDER_INIT_SIZE;
    } else {
        newInputHolderSize = *bufferSize * EXPAND_COEF;
    }

    newInputHolder = realloc(*inputHolder, newInputHolderSize);

    if(newInputHolder == REALLOC_ERROR){
        perror("expandInputBuffer. There are problems with realloc");
        return STATUS_FAILURE;
    }

    *inputHolder = newInputHolder;
    *bufferSize = newInputHolderSize;

    return STATUS_SUCCESS;
}

int readLine(char **line, size_t * currentLineLength){
    char * inputHolder = NULL;
    char * fgetsStatus = NULL;

    size_t bufferSize = 0;
    size_t currentBufferPos = 0;

    do {
        if(bufferSize == 0 || inputHolder == NULL || currentBufferPos == bufferSize - 1){
            int expandBufferStatus = expandInputBuffer(&inputHolder, &bufferSize);
            if(expandBufferStatus == STATUS_FAILURE){
                return STATUS_FAILURE;
            }
        }
        fgetsStatus = fgets(&inputHolder[currentBufferPos], bufferSize - currentBufferPos, stdin);
        if(fgetsStatus == FGETS_ERROR){
            return STATUS_FAILURE;
        }

        currentBufferPos += strlen(&inputHolder[currentBufferPos]);
    } while(inputHolder[currentBufferPos-1] != NEWLINE_CHARACTER);

    *currentLineLength = currentBufferPos;
    *line = inputHolder;

    return STATUS_SUCCESS;
}

void truncateNewLineCharacter(char *line, size_t * lineLength){
    if(*lineLength == 0 || line[*lineLength-1] != NEWLINE_CHARACTER){
        return;
    }
    line[*lineLength-1] = ENDSTRING_CHARACTER;
    *lineLength -= 1;
}

int readPattern(char ** pattern){
    char *tempLine = NULL;
    size_t currentLineLength = 0;

    printf("Enter your pattern: ");
    int readLineStatus = readLine(&tempLine, &currentLineLength);

    if(readLineStatus == STATUS_FAILURE){
        fprintf(stderr, "readPattern. There are problems with reading line\n");
        return STATUS_FAILURE;
    }

    truncateNewLineCharacter(tempLine, &currentLineLength);
    *pattern = tempLine;

    return STATUS_SUCCESS;
}

int main(){
    char *pattern = NULL;

    int readPatternStatus = readPattern(&pattern);
    if(readPatternStatus == STATUS_FAILURE){
        fprintf(stderr, "There are problems with reading pattern\n");
        return EXIT_FAILURE;
    }

    int printSuitableStringsStatus = printSuitableStrings(pattern);
    if(printSuitableStringsStatus == STATUS_FAILURE){
        return EXIT_FAILURE;
    }

    correctCompletion(pattern);

    return EXIT_SUCCESS;
}