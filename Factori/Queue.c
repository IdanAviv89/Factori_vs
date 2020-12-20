//
// Created by Idan Aviv on 17/12/2020.
//

#include "Queue.h"

char increase_queue_size(Queue *queue){


    int *new_array = (int *) malloc((queue->size + QUEUE_BLOCK_SIZE) * sizeof(int));  // allocating a new block of memory that will increase the size of the queue by QUEUE_BLOCK_SIZE

    if(new_array == NULL){ // checking if allocation succeed
        return FAIL;
    }

    for (int i = 0; i < queue->size; ++i) { // coping items to the new array
        if(queue->first_index == queue->size){
            queue->first_index = 0;
        }

        new_array[i] = queue->items[queue->first_index];
        queue->first_index += 1;
    }

    queue->first_index = 0;
    queue->last_index = queue->size - 1;
    queue->size = queue->size +QUEUE_BLOCK_SIZE;
    free(queue->items);
    queue->items = new_array;
    return SUCCEED;
}


Queue* InitializeQueue(){
    Queue *queue;
    if((queue = (Queue *) malloc(sizeof(Queue)))== NULL){
        return NULL;
    }
    queue->last_index = FAIL;
    queue->first_index = FAIL;
    queue->size = QUEUE_BLOCK_SIZE;
    if((queue->items = (int *) malloc(sizeof(int)*QUEUE_BLOCK_SIZE)) == NULL){
        return NULL;
    }
    return queue;
}

int Top(Queue *queue){

    if(queue == NULL){
        return FAIL;
    }

    if(queue->first_index == FAIL){ // queue is empty
        return FAIL;
    }
    return queue->items[queue->first_index];
}

int Pop(Queue *queue){

    if(queue == NULL){
        return FAIL;
    }

    int temp;

    if(queue->first_index == FAIL){
        return FAIL;
    }

    temp = queue->items[queue->first_index];

    if(queue->first_index == queue->last_index){ // there is one item left, returning it and empty queue
        queue->first_index = FAIL;
        queue->last_index = FAIL;
    }else{
        if(queue->first_index == queue->size){
            queue->first_index = 0;
        }else{
            queue->first_index +=1;
        }
    }

    return temp;
}

int Push(Queue *queue, int item){

    if(queue == NULL){
        return FAIL;
    }

    int number_of_items = (queue->first_index>queue->last_index) ? (queue->size - queue->first_index + queue->last_index) : (queue->last_index - queue->first_index + 1);

    if(number_of_items == queue->size) {
        if (increase_queue_size(queue) == FAIL) {
            return FAIL;
        }
    }

    if(queue->first_index == FAIL){ // first item in queue
        queue->first_index = 0;
        queue->last_index = 0;
        queue->items[queue->last_index] = item;
    }else{
        queue->last_index +=1;
        if (queue->last_index == queue->size){
            queue->last_index = 0;
        }
        queue->items[queue->last_index] = item;
    }
    return SUCCEED;

    /*if(number_of_items == queue->size){
        if (increase_queue_size(queue) == FAIL){
            return FAIL;
        }else{
            queue->first_index = 0;
            queue->last_index += 1;
            queue->items[queue->last_index] = item;
            return item;
        }
    }else{
        queue->last_index +=1;
        if (queue->last_index == queue->size){
            queue->last_index = 0;
        }
        queue->items[queue->last_index] = item;
        return item;
    }*/

}


int Empty(Queue *queue){
    if(queue == NULL){
        return FAIL;
    }

    return queue->first_index == FAIL;
}

void DestroyQueue(Queue **queue){

    if(queue != NULL) {
        free((*queue)->items);
        free(*queue);
        *queue = NULL;
    }
}

