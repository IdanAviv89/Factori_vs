//
// Created by idana on 17/12/2020.
//

#include "Lock.h"



Lock *InitializeLock(){

   Lock *lock;

    if((lock = (Lock *) malloc(sizeof (Lock))) == NULL){
        return NULL;
    }

    lock->counter = 0;
    lock->turnstile = CreateMutexA(NULL,FALSE,NULL);
    lock->read_enable= CreateMutexA(NULL,FALSE,NULL);
    lock->write_enable = CreateSemaphoreA(NULL,1,1,NULL);

   if(lock->turnstile == NULL || lock->read_enable == NULL || lock->write_enable == NULL){
       printf("ERROR: unable to initialize lock %d\n",GetLastError());
       return NULL;
   }


    return lock;
}


char read_lock(Lock *lock){

    if(lock == NULL){
        return FAIL;
    }

    if(wait_for_handle(lock->turnstile) == FAIL){
        return FAIL;
    }
    if(release_mutex(lock->turnstile) == FAIL){
        return FAIL;
    }

    if(wait_for_handle(lock->read_enable) == FAIL){
        return FAIL;
    }
        lock->counter +=1;

        if(lock->counter == 1){
            if(wait_for_handle(lock->write_enable) == FAIL){
                return FAIL;
            }
        }
    if(release_mutex(lock->read_enable) == FAIL){
        return FAIL;
    }

    return SUCCEED;
}


char read_release(Lock *lock) {
    if(lock == NULL){
        return FAIL;
    }
    if(wait_for_handle(lock->read_enable) == FAIL){
        return FAIL;
    }
    lock->counter -=1;

    if(lock->counter == 0){
        if(release_semaphore(lock->write_enable) == FAIL){
            return FAIL;
        }
    }
    if(release_mutex(lock->read_enable) == FAIL){
        return FAIL;
    }

    return SUCCEED;
}


char write_lock(Lock *lock){

    if(lock == NULL){
        return FAIL;
    }
    if(wait_for_handle(lock->turnstile) == FAIL){
        return FAIL;
    }
    if(wait_for_handle(lock->write_enable) == FAIL){
        return FAIL;
    }

    return SUCCEED;
}


char write_release(Lock *lock){
    if(lock == NULL){
        return FAIL;
    }
    if(release_mutex(lock->turnstile) == FAIL){
        return FAIL;
    }
    if(release_semaphore(lock->write_enable) == FAIL){
        return FAIL;
    }

    return SUCCEED;
}


void DestroyLock(Lock **p_p_lock){

    if(p_p_lock != NULL){
        return;
    }
    Lock *p_lock = *p_p_lock;

    if (CloseHandle(p_lock->turnstile) == 0){ // close handle
        printf("Error: unable to close handle (error code: %d)",GetLastError());
    }
    if (CloseHandle(p_lock->write_enable) == 0){ // close handle
        printf("Error: unable to close handle (error code: %d)",GetLastError());
    }
    if (CloseHandle(p_lock->read_enable) == 0){ // close handle
        printf("Error: unable to close handle (error code: %d)",GetLastError());
    }

    free(*p_p_lock);

    *p_p_lock = NULL;
}