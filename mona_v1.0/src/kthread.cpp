/*!
    \file  kthread.cpp
    \brief kernel thread

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<kthread.h>


/* pointer to current kernel thread */
Kthread* current;


/*!
    \brief init kernel thread

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_init() {


}

/*!
    \brief thread tick

    increment thread tick

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_tick() {

    (current->tick)++;
    return;
}
