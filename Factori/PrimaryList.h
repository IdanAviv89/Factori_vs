//
// Created by idana on 20/12/2020.
//

#ifndef FACTORI_PRIMARYLIST_H
#define FACTORI_PRIMARYLIST_H

#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "HardCodedData.h"

typedef struct primary_list_struct{
    int *counter_array;
    int *primary_array;
    int last_index;
    int num;
    char *str;
}primary_list;


primary_list* calculate_primary_numbers(primary_list* list);
primary_list* DestroyPrimaryList(primary_list *list);


#endif //FACTORI_PRIMARYLIST_H
