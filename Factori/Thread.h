#ifndef FACTORI_THREAD_H
#define FACTORI_THREAD_H

#include "windows.h"
#include "HelperFunctions.h"
#include "Queue.h"
#include "Lock.h"
#include "PrimaryList.h"

typedef struct thread_pram {

    // shared values:
    char* mission_filename;
    char* priority_filename;

    // global values:
    Lock* queue_lock;
    Lock* file_lock;
    Queue* queue;

    // unique values:
    int priority_start_line;
    int priority_end_line;
    int thread_id;
    // semaphores
    HANDLE previous_thread_finish_priorities;
    HANDLE thread_finish_priorities;

    // local handles:
    HANDLE priority_file;
    HANDLE mission_file;

    Queue* local_queue;
    primary_list* data;

}Thread;

DWORD WINAPI thread_main(LPVOID lpParam);




#endif //FACTORI_THREAD_H