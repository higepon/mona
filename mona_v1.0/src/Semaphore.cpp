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
#include<monaKernel.h>

/*!
    \brief up

    \param  sem target semaphore
    \author HigePon
    \date   create:2003/01/31 update:2003/02/01
*/
int Semaphore::up(semaphore* sem) {

    (*sem)++;
    return 0;
}

/*!
    \brief down

    \param  sem target semaphore
    \author HigePon
    \date   create:2003/01/31 update:
*/
int Semaphore::down(semaphore* sem) {

    disableInterrupt();

    if (*sem) {
        (*sem)--;
        enableInterrupt();
        return 0;
    } else {
        enableInterrupt();
        return -1;
    }
}

