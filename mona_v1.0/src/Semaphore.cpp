/*!
    \file  Semaphore.cpp
    \brief class Semaphore

    class Semaphore

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/01/30 update:$Date$
*/

#include<semaphore.h>
#include<kernel.h>
#include<io.h>
#include<global.h>

/*!
    \brief up

    \param  sem target semaphore
    \author HigePon
    \date   create:2003/01/31 update:2003/02/01
*/
int semaphore_up(semaphore* sem) {

    (*sem)++;
    return 0;
}

/*!
    \brief down

    \param  sem target semaphore
    \author HigePon
    \date   create:2003/01/31 update:2003/03/21
*/
int semaphore_down(semaphore* sem) {

    dword eflags = get_eflags();
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

