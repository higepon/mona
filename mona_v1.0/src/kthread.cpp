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

/*!
    \brief add thread to run

    \param thread thraed added to run

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kethread_add_to_run(Kthread* thread) {

}

/*!
    \brief remove thread from run

    \param thread thraed removed from run

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kethread_remove_from_run(Kthread* thread) {

}
