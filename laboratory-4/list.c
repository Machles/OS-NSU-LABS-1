#include "list.h"

Node* createNode(char* string){
    Node* node = (Node*) malloc(sizeof(Node));

    if(node == NULL){
        perror("There is a trouble with allocating memory with malloc for new list node");
        return NULL;
    }

    int strLength = strlen(string);

    char * newStr = (char*) malloc(strLength + 1);

    if(newStr == NULL){
        perror("There is a trouble with allocating memory with malloc for new node's string");
        /// функция создания нового элемента списка не должна завершать процесс. Верните ошибку, обработайте выше,
        /// все ранее выделенные ресурсы должны быть освобождены явным образом
        free(node);
        return NULL;
    }

    memccpy(newStr, string, strLength, sizeof(char)*strLength );

    node->next = NULL;
    node->stringValue = newStr;
    newStr[strLength] = '\0';

    return node;
}

int push(List* list, Node* node){

    if(node == NULL){
        fprintf(stderr, "Invalid node\n");
        free(list);
        return STATUS_FAIL;
    }

    if(list == NULL){
        /// функция добавления элемента в список не должна отвечать за его создание, если список не создан - просто верните
        /// соответствующую ошибку и обработайте выше
        fprintf(stderr, "Invalid list\n");
        free(node);
        return STATUS_FAIL;
    }

    if(list->head == NULL){
        list->head = node;
        list->length += 1;
        return STATUS_SUCCESS;
    }

    node->next = list->head;
    list->head = node;
    list->length += 1;

    return STATUS_SUCCESS;
}

void initList(List* list){
    list->head = NULL;
    list->length = 0;
}

void printList(List* list){
    if(list->head == NULL){
        printf("%s", "List is empty!");
        return;
    }

    Node * current = list->head;

    while(current != NULL) {
        printf("%s", current->stringValue);
        current = current->next;
    }

    printf("List length %d.", list->length);
}

void freeNode(Node* node){
    free(node->stringValue);
    free(node);
    node = NULL;
}

void freeList(List* list){
    Node *current = list->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        freeNode(temp);
    }
    list = NULL;
}