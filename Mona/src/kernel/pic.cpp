/*!
    \file  pic.cpp
    \brief pic functions

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/08 update:$Date$
*/

#include "pic.h"

/*!
    \brief PIC initilaize


    \author HigePon
    \date   create:2003/03/08 update:
*/
#define PIT_MODE   0x43
#define PIT_COUNT0 0x40
void pic_init() {

    /* master */
    outp8(0x20, 0x11); /* ICW1 */
    outp8(0x21, 0x60); /* ICW2 */ /* IRQ 0h = int 60h */
    outp8(0x21, 0x04); /* ICW3 */
    outp8(0x21, 0x01); /* ICW4 */

    /* slave */
    outp8(0xa0, 0x11); /* ICW1 */
    outp8(0xa1, 0x28); /* ICW2 */ /* IRQ  8h = int 28h */
    outp8(0xa1, 0x02); /* ICW3 */
    outp8(0xa1, 0x01); /* ICW4 */

    /*
       mask all interrupts

       this should be done after controller initialize.
       because qemu set imr set unmasked when initialize controller
    */
    outp8(0x21, 0xff);
    outp8(0xA1, 0xff);

    /* timer settings 10ms */
    //    setTimerInterval(10);
    /* timer settings 10ms */
    dword timer_count = 10 * 1193181 / 1000;
    outp8(PIT_MODE, 0x36);
    outp8(PIT_COUNT0, timer_count & 0xff);
    outp8(PIT_COUNT0, timer_count >> 8);

    /* no user interrupt handler yet */
    for (int i = 0; i < 16; i++)
    {
        g_irqInfo[i].hasUserHandler = false;
    }

    return;
}

/*!
    \brief setTimerInterval

    \param ms interval ms

    \author HigePon
    \date   create:2004/01/11 update:
*/
void setTimerInterval(dword ms) {
    dword timerCounter = (dword)(ms * 1193.18);
    outp8(PIT_MODE, 0x34);
    outp8(PIT_COUNT0, timerCounter & 0xff);
    outp8(PIT_COUNT0, timerCounter >> 8);
}

/*!
    \brief disable timer

    disable timer interrupt
    mask IMR bit 0

    \author HigePon
    \date   create:2003/01/12 update:
*/
void disableTimer() {

    outp8(0x21, inp8(0x21) | 0x01);
}

/*!
    \brief enable timer

    enable timer interrupt

    \author HigePon
    \date   create:2003/01/12 update:
*/
void enableTimer() {

    outp8(0x21, inp8(0x21) & 0xFE);
}

/*!
    \brief disable keyboard interrupt

    \author HigePon
    \date   create:2003/03/08 update:
*/
void disableKeyboard() {

    outp8(0x21, inp8(0x21) | 0x02);
}

/*!
    \brief enable keyboard interrupt

    \author HigePon
    \date   create:2003/03/08 update:
*/
void enableKeyboard() {

    outp8(0x21, inp8(0x21) & 0xFD);
}

/*!
    \brief enable FDC interrupt

    \author HigePon
    \date   create:2004/02/08 update:
*/
void enableFDC() {

    outp8(0x21, inp8(0x21) & 0xBF);
}

