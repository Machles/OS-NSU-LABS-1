#ifndef LABORATORIES_1ST_PART_LIST_H
#define LABORATORIES_1ST_PART_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STATUS_FAIL -1
#define STATUS_SUCCESS 0

typedef struct Node{
    char * stringValue;
    struct Node * next;
} Node;

typedef struct List{
    Node * head;
    int length;
} List;

Node * createNode(char * string);
int push(List * list, Node * node);

void initList(List * list);
void printList(List * list);

void freeNode(Node * node);
void freeList(List * list);

#endif //LABORATORIES_1ST_PART_LIST_H
