//
// Created by idana on 19/12/2020.
//

#include "HelperFunctions.h"




char wait_for_handle(HANDLE waiting_object){
    DWORD dwWaitResult;

    dwWaitResult = WaitForSingleObject(
            waiting_object,
            waiting_max_time);

    switch (dwWaitResult){
        case WAIT_OBJECT_0: // waiting is done
            return SUCCEED;
        default:  // An error occurred
            printf("Error: waiting to handle error (error code: %d)\n", GetLastError());
            return FAIL;
    }

}

char release_mutex(HANDLE mutex){
    if (ReleaseMutex(mutex) == 0){
        printf("Error: unable to release mutex (error code: %d)",GetLastError());

        return FAIL;
    }

    return SUCCEED;
}


char release_semaphore(HANDLE semaphore){

    if (!ReleaseSemaphore(
            semaphore,  // handle to semaphore
            1,            // increase count by one
            NULL) )       // not interested in previous count
    {
        printf("Release Semaphore error: %d\n", GetLastError());
        return FAIL;
    }

    return SUCCEED;
}