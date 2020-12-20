#include "PrimaryList.h"



int primary_counter(int* num, int primary) {
    int counter = 0;
    while ((*num) % primary == 0) {
        *num = (*num) / primary;
        counter++;
    }

    return counter;
}

char formatted_string(primary_list* list) {

    int size = STRING_BLOCK_SIZE;
    if ((list->str = (char*)malloc(sizeof(char) * STRING_BLOCK_SIZE)) == NULL) {
        return FAIL;
    }


    if (sprintf(list->str, "The prime factors of %d are: ", list->num) < 0) {
        return FAIL;
    }

    int last_index = strlen(list->str);
    size = size - (last_index + 1);



    for (int i = 0; i <= list->last_index; ++i) {
        for (int j = 0; j < list->counter_array[i]; ++j) {

            if (size < NUM_BUFFER) {
                if ((list->str = (char*)realloc(list->str, (size + STRING_BLOCK_SIZE) * sizeof(char))) == NULL) {
                    return FAIL;
                }
                size += STRING_BLOCK_SIZE;
            }

            if (i == list->last_index && j == list->counter_array[i] - 1) {
                if (sprintf(list->str + last_index, "%d", list->primary_array[i]) < 0) {
                    return FAIL;
                }
            }
            else {
                if (sprintf(list->str + last_index, "%d, ", list->primary_array[i]) < 0) {
                    return FAIL;
                }
            }

            last_index = strlen(list->str);
        }
    }

    if (size < NUM_BUFFER) {
        if ((list->str = (char*)realloc(list->str, (size + STRING_BLOCK_SIZE) * sizeof(char))) == NULL) {
            return FAIL;
        }
        size += STRING_BLOCK_SIZE;
    }

    if (sprintf(list->str + last_index, "\r\n") < 0) {
        return FAIL;
    }

    return SUCCEED;

}


primary_list* calculate_primary_numbers(int num) {

    int array_size = NUM_PRIMARY_NUMBERS;
    int temp;
    primary_list* list;

    if ((list = (primary_list*)malloc(sizeof(primary_list))) == NULL) {
        return NULL;
    }
    list->num = num;
    list->last_index = -1;
    
    if ((list->counter_array = (int*)malloc(array_size * sizeof(int))) == NULL) {
        return NULL;
    }

    if ((list->primary_array = (int*)malloc(array_size * sizeof(int))) == NULL) {
        return NULL;
    }

    if ((temp = primary_counter(&num, 2)) != 0) {
        (list->last_index)++;
        (list->counter_array)[list->last_index] = temp;
        (list->primary_array)[list->last_index] = 2;
    }

    for (int i = 3; i <= floor(sqrt(num)); i = i + 2) {
        if ((temp = primary_counter(&(num), i)) != 0) {
            (list->last_index)++;
            if (list->last_index >= array_size) {
                array_size += NUM_PRIMARY_NUMBERS;
                if((list->counter_array = (int*)realloc(list->counter_array, array_size * sizeof(int))) == NULL) {
                    return NULL;
                }
                if ((list->primary_array = (int*)realloc(list->primary_array, array_size * sizeof(int))) == NULL) {
                return NULL;
                }
            }

            (list->counter_array)[list->last_index] = temp;
            (list->primary_array)[list->last_index] = i;
        }
    }

    if (num > 2) {
        (list->last_index)++;
        if (list->last_index >= array_size) {
            array_size += 1;
            if ((list->counter_array = (int*)realloc(list->counter_array, array_size * sizeof(int))) == NULL) {
                return NULL;
            }
            if ((list->primary_array = (int*)realloc(list->primary_array, array_size * sizeof(int))) == NULL) {
                return NULL;
            }
        }
        (list->counter_array)[list->last_index] = 1;
        (list->primary_array)[list->last_index] = num;
    }

    if (formatted_string(list) == FAIL) {
        return DestroyPrimaryList(list);
    }

    return list;
}


primary_list* DestroyPrimaryList(primary_list* list) {

    if (list == NULL) {
        return NULL;
    }

    if (list->counter_array != NULL) {
        free(list->counter_array);
    }
    if (list->primary_array != NULL) {
        free(list->primary_array);
    }
    if (list->str != NULL) {
        free(list->str);
    }

    free(list);
    return NULL;
}


