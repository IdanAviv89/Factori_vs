//
// Created by idana on 17/12/2020.
//

#ifndef FACTORI_QUEUE_H
#define FACTORI_QUEUE_H

#include "HardCodedData.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct queue_struct{
    int *items;
    int size;
    int first_index;
    int last_index;
} Queue;


Queue* InitializeQueue();
int Top(Queue *queue);
int Pop(Queue *queue);
int Push(Queue *queue, int item);
void DestroyQueue(Queue **queue);


#endif //FACTORI_QUEUE_H
