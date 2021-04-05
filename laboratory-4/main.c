#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include <errno.h>

#define INPUT_HOLDER_SIZE 1024
#define BUFFER_SIZE 10
#define TRUE 1

extern int errno;

int main() {
    char stopSymbol = '.';
    unsigned long currentStringLength = 0;
    unsigned long currentBufferSize = 0;
    char* readingResult = NULL;
    char* inputHolder = NULL;
    char buffer[BUFFER_SIZE];

    List* list = (List*) malloc(sizeof(List));
    if(list == NULL){
        perror("There is a trouble with allocating memory with malloc for list");
        exit(EXIT_FAILURE);
    }

    initList(list);

    do {
        readingResult = fgets(buffer, BUFFER_SIZE, stdin);
        if(readingResult == NULL){
            break;
        }

        currentBufferSize = strlen(buffer);

        if(currentBufferSize < 0){
            fprintf(stderr,"There is a problem with getting string length");
            exit(EXIT_FAILURE);
        }

        char * tempHolder = realloc(inputHolder, currentStringLength+currentBufferSize+1);
        if(tempHolder == NULL){
            perror("There is a trouble with reallocating memory with realloc");
            exit(EXIT_FAILURE);
        }
        inputHolder = tempHolder;

        strcpy(inputHolder+currentStringLength, buffer);
        currentStringLength += currentBufferSize;

        if(buffer[currentBufferSize-1] != '\n'){
            continue;
        }

        if(inputHolder[0] == stopSymbol){
            break;
        }

        /// Ошибки из createNode никак не обрабатываются - теперь отбработчик снизу
        Node * node = createNode(inputHolder);

        if(node == NULL){
            fprintf(stderr, "Node hasn't been created");
            exit(EXIT_FAILURE);
        }

        int status = push(list, node);

        if(status == STATUS_FAIL){
            fprintf(stderr, "There are problems with pushing node");
            exit(EXIT_FAILURE);
        }

        currentStringLength = 0;

    } while(TRUE);

    if (readingResult == NULL && errno !=0){
        perror("There is troubles with reading from stream");
        exit(EXIT_FAILURE);
    }

    printList(list);

    // free() не устанавливает errno в случае ошибки
    free(inputHolder);
    freeList(list);

    return EXIT_SUCCESS;
}