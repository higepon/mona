/*!
    \file  semaphore.h
    \brief semaphore

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/01/30 update:$Date$
*/
#ifndef _MONA_SEMAPHORE_
#define _MONA_SEMAPHORE_

typedef int semaphore;

int semaphore_up(semaphore*);
int semaphore_down(semaphore*);

#endif
