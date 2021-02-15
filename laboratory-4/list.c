#include "list.h"

Node * createNode(char * string){
    Node * node = (Node*) malloc(sizeof(Node));

    int strLength = strlen(string);

    char * newStr = (char*) malloc(strLength + 1);

    memccpy(newStr, string, strLength, sizeof(char)*strLength );

    node->next = NULL;
    node->stringValue = newStr;

    return node;
}

void push(List * list, Node * node){

    if(list->head == NULL){
        list->head = node;
        return;
    }

    Node * current = list->head;
    while(current->next != NULL){
        current = current->next;
    }

    current->next = node;
    list->length += 1;
}

void initList(List * list){
    list->head = NULL;
    list->length = 0;
}

void printList(List * list){
    if(list->head == NULL){
        printf("%s", "List is empty!");
        return;
    }

    Node * current = list->head;

    while(current != NULL){
        printf("%s\n", current->stringValue);
        current = current->next;
    }
}

void freeNode(Node * node){
    free(node->stringValue);
    free(node);
}

void freeList(List * list){
    Node *current = list->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        freeNode(temp);
    }
}