#include <stdio.h>
#include <malloc.h>
#include "list.h"
#include "string.h"

#define INPUT_HOLDER_SIZE 1024

/*
 * Напишите программу, которая вставляет строки, введенные с клавиатуры, в список. Память под узлы списка выделяйте динамически с использованием malloc(3).
 * Ввод завершается, когда в начале строки вводится точка (.). Затем все строки из списка выводятся на экран.
 * Подсказка: Объявите массив символов размера, достаточного чтобы вместить самую длинную введенную строку. Используйте fgets(3), чтобы прочитать строку,
 * и strlen(3), чтобы определить ее длину. Помните, что strlen(3) не считает нулевой символ, завершающий строку.
 * После определения длины строки, выделите блок памяти нужного размера и внесите новый указатель в список.*/

int main() {
    char stopSymbol = '.';
    int currentStringLength = 0;

    char* inputHolder = (char*) malloc(INPUT_HOLDER_SIZE + 1);
    if(inputHolder == NULL){
        perror("There is a trouble with allocating memory with malloc for inputHolder.\n");
        exit(EXIT_FAILURE);
    }

    List* list = (List*) malloc(sizeof(List));
    if(list == NULL){
        perror("There is a trouble with allocating memory with malloc for list.\n");
        exit(EXIT_FAILURE);
    }

    initList(list);

    while (fgets(inputHolder, INPUT_HOLDER_SIZE, stdin)){
        currentStringLength = ( (int) strlen(inputHolder) ) - 1;
        inputHolder[currentStringLength] = '\0'; /* Избавились от \n, заменив на \0 */

        if(inputHolder[0] == stopSymbol){
            break;
        } else {
            push(list, createNode(inputHolder) );
        }

    }

    printList(list);

    // free() не устанавливает errno в случае ошибки
    free(inputHolder);
    freeList(list);

    return EXIT_SUCCESS;
}