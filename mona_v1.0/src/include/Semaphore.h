/*!
    \file  Semaphore.h
    \brief class Semaphore

    class Semaphore

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/01/30 update:$Date$
*/
#ifndef _MONA_SEMAPHORE_
#define _MONA_SEMAPHORE_

typedef int semaphore;

/*!
    Semaphore class
*/
class Semaphore {

  private:
    Semaphore();
    ~Semaphore();
  public:
    static int up(semaphore*);
    static int down(semaphore*);
};


#endif
