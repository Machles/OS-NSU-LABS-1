#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATUS_SUCCESS 0
#define STATUS_FAILURE -1

#define REALLOC_ERROR NULL
#define FGETS_ERROR NULL

#define NEWLINE_CHARACTER '\n'
#define ENDSTRING_CHARACTER '\0'

#define INPUT_HOLDER_INIT_SIZE 64
#define EXPAND_COEF 2

/*int findSuitableStrings(){

}*/

int expandInputBuffer(char** inputHolder, size_t *bufferSize){
    char *newInputHolder = NULL;
    int newInputHolderSize;

    if(*bufferSize == 0){
        newInputHolderSize = INPUT_HOLDER_INIT_SIZE;
    } else {
        newInputHolderSize *= EXPAND_COEF;
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

    printf("%s", pattern);

/*    int findSuitableStringsStatus = findSuitableStrings();
    if(findSuitableStringsStatus == STATUS_FAILURE){
        return EXIT_FAILURE;
    }*/

    return EXIT_SUCCESS;
}