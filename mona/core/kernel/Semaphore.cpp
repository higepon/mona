/*!
    \file  Semaphore.cpp
    \brief class Semaphore

    class Semaphore

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2003/01/30 update:$Date$
*/

#include "Semaphore.h"
#include "kernel.h"
#include "io.h"
#include "global.h"

/*!
    \brief up

    \param  sem target semaphore
    \author Higepon
    \date   create:2003/01/31 update:2003/02/01
*/
void Semaphore::up(semaphore* sem) {

    (*sem)++;
    return;
}

/*!
    \brief down

    \param  sem target semaphore
    \author Higepon
    \date   create:2003/01/31 update:2003/03/21
*/
int Semaphore::down(semaphore* sem) {

    uint32_t eflags = get_eflags();
    disableInterrupt();

    if (*sem) {
        (*sem)--;
        set_eflags(eflags);
        return 0;
    } else {
        set_eflags(eflags);
        return -1;
    }
}

