//
// Created by idana on 19/12/2020.
//

#include "Thread.h"

char release_thread_memory(Thread *thread){
    char exist_code = SUCCEED;
    

    if (thread->priority_file != INVALID_HANDLE_VALUE) {
        exist_code |= (CloseHandle(thread->priority_file) == 0) ? FAIL : SUCCEED;
    }

    DestroyQueue(&(thread->local_queue));
    DestroyPrimaryList(thread->data);

    return exist_code;
}





char open_priority_file(Thread *thread){ // open priority file in the correct location

    thread->priority_file = CreateFileA(thread->priority_filename,   // name of the file
                             GENERIC_READ ,          // open for writing
                             FILE_SHARE_READ,        // share for reading
                             NULL,                   // default security
                             OPEN_EXISTING,          // open existing file
                             FILE_ATTRIBUTE_NORMAL,  // normal file
                             NULL);                  // no attr. template

    if (thread->priority_file == INVALID_HANDLE_VALUE){
        printf("ERROR: unable to open priority file (error code %d)",GetLastError());
        release_thread_memory(thread);
        return FAIL;
    }

    int line_counter = 0;
    BYTE c = '\0';
    DWORD  dwBytesRead = 0;

    while(line_counter < thread->priority_start_line){ // moving the pointer to the right location
        if(ReadFile(thread->priority_file,&c,1,&dwBytesRead,NULL) == 0){
            printf("ERROR: unable to read priority file (error code %d)",GetLastError());
            release_thread_memory(thread);
            return FAIL;
        }
        if(c == '\n'){
            line_counter++;
        }
    }
    return SUCCEED;
}



char write_to_file(Thread *thread, int num){

    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten = 0;
    DWORD dwBytesToWrite = (DWORD) strlen(thread->data->str);



    if (SetFilePointer(thread->mission_file,0,NULL,FILE_END) == INVALID_SET_FILE_POINTER) // Test for failure
    {
        printf("Error: unable to move pointer to the correct bit in the mission file (error code:%d)", GetLastError());
        release_thread_memory(thread);
        return FAIL;
    }


    bErrorFlag = WriteFile(
            thread->mission_file,           // open file handle
            thread->data->str,      // start of data to write
            dwBytesToWrite,  // number of bytes to write
            &dwBytesWritten, // number of bytes that were written
            NULL);            // no overlapped structure

    if (FALSE == bErrorFlag){
        printf("Terminal failure: Unable to write to file.\n");
        release_thread_memory(thread);
        return FAIL;
    }
    else if (dwBytesWritten != dwBytesToWrite){
            printf("Error: dwBytesWritten != dwBytesToWrite\n");
        release_thread_memory(thread);
            return FAIL;
    }


    return SUCCEED;
}



char calculate_primary(Thread *thread,int start_byte){

    if (thread->mission_file == NULL) {
        thread->mission_file = CreateFileA(thread->mission_filename,   // name of the file
            (GENERIC_READ | GENERIC_WRITE),          // open for writing
            FILE_SHARE_WRITE | FILE_SHARE_READ,       // do not share
            NULL,                   // default security
            OPEN_EXISTING,             // create new file only
            FILE_ATTRIBUTE_NORMAL,  // normal file
            NULL);                  // no attr. template

    }

    if (thread->mission_file == INVALID_HANDLE_VALUE)
    {
        printf("ERROR: unable to open mission file (error code %d)",GetLastError());
        release_thread_memory(thread);
        return FAIL;
    }

    BYTE c = '\0';
    DWORD  dwBytesRead = 0;
    int num = 0;

    if (SetFilePointer( thread->mission_file, start_byte, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        printf("Error: unable to move pointer to the correct bit in the mission file (error code:%d)", GetLastError());
        release_thread_memory(thread);
        return FAIL;
    }


    if(read_lock(thread->file_lock) == FAIL){
        printf("Error couldn't set read lock on mission file");
        release_thread_memory(thread);
        return FAIL;
    }

    while(TRUE){ 
        if(ReadFile(thread->mission_file,&c,1,&dwBytesRead,NULL) == 0){
            printf("ERROR: unable to read mission file (error code %d)",GetLastError());
            release_thread_memory(thread);
            return FAIL;
        }
        if(c == '\n'){
            break;
        }
        if(c >= '0' && c <= '9'){
            num = num*10+c-'0';
        }
    }

    if(read_release(thread->file_lock) == FAIL){
        printf("Error couldn't release read of the mission file");
        release_thread_memory(thread);
        return FAIL;
    }

  

    if ((thread->data = calculate_primary_numbers(num)) == NULL) {
        printf("Error calculating the primary numbers");
        release_thread_memory(thread);
        return FAIL;
    }



    if(write_lock(thread->file_lock) == FAIL){
        printf("Error couldn't lock the mission file for write");
        release_thread_memory(thread);
        return FAIL;
    }

    if(write_to_file(thread,num) == FAIL){
        release_thread_memory(thread);
        return FAIL;
    }

    if(write_release(thread->file_lock) == FAIL){
        printf("Error couldn't release the mission file from write");
        release_thread_memory(thread);
        return FAIL;
    }

    return SUCCEED;
}


DWORD WINAPI thread_main(LPVOID lpParam){

    /// transforming the input of the function to the appropriate format

    // Check if lpParam is NULL
    if (lpParam == NULL) {
        return FAIL;
    }

    Thread *thread= (Thread *) lpParam;
    ///


    /// read priority file
    open_priority_file(thread);
    ///

    thread->local_queue = InitializeQueue();

    int line_counter = thread->priority_start_line;
    BYTE c ='\0';
    DWORD  dwBytesRead = 0;
    int temp = 0;


    while(line_counter <= thread->priority_end_line){
        if (ReadFile(thread->priority_file,&c,1,&dwBytesRead,NULL) == 0){
            printf("ERROR: unable to read priority file (error code %d)",GetLastError());
            release_thread_memory(thread);
            return FAIL;
        }

        if(c == '\n'){
            Push(thread->local_queue ,temp);
            temp = 0;
            line_counter++;
        }

        if(c >= '0' && c <= '9'){
            temp = temp*10 + c-'0';
        }

    }

    /// waiting for the previous thread to finish


    if(wait_for_handle(thread->previous_thread_finish_priorities) == FAIL){
        release_thread_memory(thread);
        return FAIL;
    }

    if(write_lock(thread->queue_lock) == FAIL){
        release_thread_memory(thread);
        return FAIL;
    }
    /// inserting values to the queue
    while((temp = Pop(thread->local_queue)) != FAIL){
        Push(thread->queue,temp);
    }

    if(write_release(thread->queue_lock) == FAIL){
        release_thread_memory(thread);
        return FAIL;
    }

    if(release_semaphore(thread->thread_finish_priorities) == FAIL){
        release_thread_memory(thread);
        return FAIL;
    }
    ///


    /// reading the queue and start doing the job

    while(TRUE) {
        if(write_lock(thread->queue_lock) == FAIL){
            release_thread_memory(thread);
            return FAIL;
        }
        temp = Pop(thread->queue);
        if(write_release(thread->queue_lock) == FAIL){
            release_thread_memory(thread);
            return FAIL;
        }
        if(temp == FAIL){
            break;
        }
        if (calculate_primary(thread,temp) == FAIL){
            release_thread_memory(thread);
            return FAIL;
        }
    }

    
    return release_thread_memory(thread) | SUCCEED;
}