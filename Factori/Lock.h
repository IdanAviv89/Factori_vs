//
// Created by idana on 17/12/2020.
//

#ifndef FACTORI_LOCK_H
#define FACTORI_LOCK_H

#include "windows.h"
#include <stdio.h>
#include "HardCodedData.h"
#include "HelperFunctions.h"
typedef struct lock_struct{

    HANDLE turnstile;
    HANDLE write_enable;
    HANDLE read_enable;
    int counter;
}Lock;

Lock *InitializeLock();

char read_lock(Lock *lock);

char read_release(Lock *lock);

char write_lock(Lock *lock);

char write_release(Lock *lock);

void DestroyLock(Lock **lock);

#endif //FACTORI_LOCK_H
