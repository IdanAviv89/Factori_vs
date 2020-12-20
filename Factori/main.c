
// importing the libraries needed
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// included models needed
#include "Queue.h"
#include "Lock.h"
#include "Thread.h"
#include "HelperFunctions.h"
#include "HardCodedData.h"


BOOL initialize_semaphores_array(HANDLE *semaphores_array, int num, BOOL first_is_on){

    if(first_is_on){
        if ((semaphores_array[0] = CreateSemaphoreA(NULL,1,1,NULL)) == NULL){
            printf("ERROR: couldn't create semaphore (error code: %d)",GetLastError());
            return FALSE;
        }
    }

    for (int i = (first_is_on)? 1:0; i<num;i++){
        if((semaphores_array[i] = CreateSemaphoreA(NULL,0,1,NULL)) == NULL){
            printf("ERROR: couldn't create semaphore (error code: %d)",GetLastError());
            return FALSE;
        }
    }

    return TRUE;
}

/// creating the semaphore. If successful return TURE else FALSE;
BOOL create_semaphores_array(int number_of_threads,HANDLE **finish_priority_semaphores){



    *finish_priority_semaphores = (HANDLE *) calloc(number_of_threads + 2,sizeof(HANDLE));

    if(*finish_priority_semaphores == NULL ){
        return FALSE;
    }

    if(!initialize_semaphores_array(*finish_priority_semaphores,(number_of_threads + 2),TRUE)){
        return FALSE;
    }

    return TRUE;
}

char Close_all_handles(HANDLE *array,int len){
    char exist_code = SUCCEED;
    if(array != NULL){
        for(int i = 0; i<len;i++){
            if(array[i] != NULL){
                if (CloseHandle(array[i]) == 0){
                    exist_code = FAIL;
                }
            }
        }
    }

    free(array);

    return exist_code;
}


/// release all memory from the main threads, return TRUE if successful else FALSE
char release_memory(Thread *shared_parameters,HANDLE *finish_priority_semaphores,HANDLE *thread_handles, int number_of_threads){

    char exist_code = SUCCEED;

    exist_code |= Close_all_handles(finish_priority_semaphores,number_of_threads+2);
    exist_code |= Close_all_handles(thread_handles,number_of_threads);


    DestroyLock(&shared_parameters->queue_lock);
    DestroyLock(&shared_parameters->file_lock);
    DestroyQueue(&shared_parameters->queue);

    return exist_code;
}

int main(int argc, char *argv[]) {

    if(argc != 5){
        printf("Program didn't receive the correct form of arguments");
        return -1;
    }


    /// creating the queue and lock that will be shared between all threads
    Lock *queue_lock = InitializeLock();
    Lock *file_lock = InitializeLock();
    Queue *queue = InitializeQueue();

    if(queue_lock == NULL || file_lock == NULL || queue == NULL){
        release_memory(NULL,NULL,NULL,0);
        return FAIL;
    }
    ///


    /// creating the shared handles
    HANDLE *finish_priority_semaphores;
    HANDLE *thread_handles;
    ///


    ///Initialize a struct that will be shared by all threads
    Thread shared_parameters = {argv[1],argv[2],queue_lock,file_lock,queue};
    ///


    /// calculating the number of lines each threads have to read from the priority file
    int number_of_tasks = atoi(argv[3]);
    int number_of_threads = atoi(argv[4]);

    int lines_per_thread = number_of_tasks / number_of_threads;
    int extra_line_thread = number_of_tasks % number_of_threads;

    int thread_start_line = 0;
    ///



    /// creating semaphores for threads communication

    if(!create_semaphores_array(number_of_threads,&finish_priority_semaphores)){
        release_memory(&shared_parameters,finish_priority_semaphores,NULL,number_of_threads);
        return FAIL;
    }
    ///

    /// Create handles for all threads
    if((thread_handles  = (HANDLE *) calloc(number_of_threads,sizeof(HANDLE))) == NULL){
        release_memory(&shared_parameters,finish_priority_semaphores,NULL,number_of_threads);
        return FAIL;
    }
    ///


    /// initialize each thread
    for (int i = 0; i < number_of_threads; i++) {

        Thread *local_parameters;

        if((local_parameters = (Thread *) malloc(sizeof(Thread))) == NULL){
            release_memory(&shared_parameters,finish_priority_semaphores,thread_handles,number_of_threads);
            return FAIL;
        }

        local_parameters-> mission_filename = argv[1];
        local_parameters-> priority_filename = argv[2];
        local_parameters-> queue_lock = queue_lock;
        local_parameters->file_lock = file_lock;
        local_parameters->queue = queue;

        /// calculating the lines the thread should read from priority file
        local_parameters->priority_start_line = thread_start_line;
        local_parameters->priority_end_line = thread_start_line+(lines_per_thread-1);
        local_parameters->thread_id  = i;
        if(extra_line_thread > 0){
            extra_line_thread -= 1;
            local_parameters->priority_end_line += 1;
        }

        thread_start_line = local_parameters->priority_end_line +1;
        ///

        /// loading the thread communications (semaphores)
        local_parameters->previous_thread_finish_priorities = finish_priority_semaphores[i];
        local_parameters->thread_finish_priorities = finish_priority_semaphores[i+1];
        ///


        DWORD threadId;
        if ((thread_handles[i] = CreateThread(NULL, 0, thread_main, local_parameters, 0, &threadId)) == NULL) {
            printf("Error: unable to create thread (Error code %d)", GetLastError());
            release_memory(&shared_parameters,finish_priority_semaphores,thread_handles,number_of_threads);
            return FAIL;
        }
    }


    /// waiting to all threads to finish
    DWORD wait_result = WaitForMultipleObjects(number_of_threads,thread_handles,TRUE,waiting_max_time);

    // checking for an error:
    if (!(wait_result >= WAIT_OBJECT_0 && wait_result <= WAIT_OBJECT_0 + number_of_threads - 1)) {

        // error happened
        if (wait_result == WAIT_TIMEOUT) {
            printf("Error: threads take too long, closing threads");
        }
        else if (wait_result == WAIT_FAILED) {
            printf("Error: wait for multiple objects failed (error code: %d)", GetLastError());
        }
        else {
            printf("Error: threads didn't release memory correctly");
        }
        release_memory(&shared_parameters,finish_priority_semaphores,thread_handles,number_of_threads);
        return FAIL;
    }
    ///


    /// check if all threads finish successfully
    BOOL exist_code = SUCCEED;
    for (int j = 0; j < number_of_threads; j++) {
        DWORD thread_finish_code;
        if (GetExitCodeThread(thread_handles[j], &thread_finish_code) == 0) {
            printf("Error: couldn't get thread number %d exist code (error code:%d)", j, GetLastError());
            release_memory(&shared_parameters,finish_priority_semaphores,thread_handles,number_of_threads);
            return FAIL;
        }
        exist_code |= thread_finish_code;
    }
    ///


    /// release memory and finish main thread
    exist_code |= release_memory(&shared_parameters,finish_priority_semaphores,thread_handles,number_of_threads);
    return (exist_code == SUCCEED) ? SUCCEED : FAIL;
}


