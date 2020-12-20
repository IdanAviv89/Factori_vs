//
// Created by idana on 19/12/2020.
//

#ifndef FACTORI_HELPERFUNCTIONS_H
#define FACTORI_HELPERFUNCTIONS_H

#include <stdio.h>
#include "windows.h"
#include "HardCodedData.h"



char wait_for_handle(HANDLE waiting_object);
char release_mutex(HANDLE mutex);
char release_semaphore(HANDLE semaphore);


#endif //FACTORI_HELPERFUNCTIONS_H
