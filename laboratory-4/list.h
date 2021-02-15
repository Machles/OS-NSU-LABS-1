#ifndef LABORATORIES_1ST_PART_LIST_H
#define LABORATORIES_1ST_PART_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node{
    char * stringValue;
    struct Node * next;
} Node;

typedef struct List{
    Node * head;
    int length;
} List;

Node * createNode(char * string);
void push(List * list, Node * node);

void initList(List * list);
void printList(List * list);

void freeNode(Node * node);
void freeList(List * list);

#endif //LABORATORIES_1ST_PART_LIST_H
