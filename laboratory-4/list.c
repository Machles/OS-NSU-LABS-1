#include "list.h"

Node* createNode(char* string){
    Node* node = (Node*) malloc(sizeof(Node));

    if(node == NULL){
        perror("There is a trouble with allocating memory with malloc for new list node.\n");
        return NULL;
    }

    int strLength = strlen(string);

    char * newStr = (char*) malloc(strLength + 1);

    if(newStr == NULL){
        perror("There is a trouble with allocating memory with malloc for new node's string.\n");
        exit(EXIT_FAILURE);
    }

    memccpy(newStr, string, strLength, sizeof(char)*strLength );

    node->next = NULL;
    node->stringValue = newStr;

    return node;
}

void push(List* list, Node* node){

    if(node == NULL){
        return;
    }

    if(list == NULL){
        list = (List*) malloc(sizeof(List));
        initList(list);
        if(list == NULL){
            perror("There is a trouble with allocating memory with malloc for list.\n");
            exit(EXIT_FAILURE);
        }
    }

    if(list->head == NULL){
        list->head = node;
        list->length += 1;
        return;
    }

    node->next = list->head;
    list->head = node;
    list->length += 1;
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

    while(current != NULL){
        printf("%s\n", current->stringValue);
        current = current->next;
    }

    printf("\nList length %d.", list->length);
}

void freeNode(Node* node){
    free(node->stringValue);
    free(node);
}

void freeList(List* list){
    Node *current = list->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        freeNode(temp);
    }
}