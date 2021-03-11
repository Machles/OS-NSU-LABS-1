#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include <errno.h>

#define INPUT_HOLDER_SIZE 1024

int main() {
    char stopSymbol = '.';
    int currentStringLength = 0;

    char* inputHolder = (char*) malloc(INPUT_HOLDER_SIZE + 1);
    if(inputHolder == NULL){
        perror("There is a trouble with allocating memory with malloc for inputHolder");
        exit(EXIT_FAILURE);
    }

    List* list = (List*) malloc(sizeof(List));
    if(list == NULL){
        perror("There is a trouble with allocating memory with malloc for list");
        exit(EXIT_FAILURE);
    }

    initList(list);

    char * readingResult = fgets(inputHolder, INPUT_HOLDER_SIZE, stdin);

    while (readingResult != NULL){
        currentStringLength = ( (int) strlen(inputHolder) ) - 1;
        inputHolder[currentStringLength] = '\0'; /* Избавились от \n, заменив на \0 */

        if(inputHolder[0] == stopSymbol){
            break;
        }

        /// Ошибки из createNode никак не обрабатываются
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

        readingResult = fgets(inputHolder, INPUT_HOLDER_SIZE, stdin);
    }

    if(readingResult == NULL){
        perror("There is troubles with reading from stream");
    }

    printList(list);

    // free() не устанавливает errno в случае ошибки
    free(inputHolder);
    freeList(list);

    return EXIT_SUCCESS;
}