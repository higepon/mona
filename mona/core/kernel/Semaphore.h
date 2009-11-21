/*!
    \file  Semaphore.h
    \brief semaphore

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/01/30 update:$Date$
*/
#ifndef _MONA_SEMAPHORE_
#define _MONA_SEMAPHORE_

typedef int semaphore;

class Semaphore {

  public:
    static void up(semaphore* sem);
    static int down(semaphore* sem);
};

#endif
