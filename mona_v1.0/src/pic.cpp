/*!
    \file  pic.cpp
    \brief pic functions

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/08 update:$Date$
*/

#include<pic.h>

/*!
    \brief PIC initilaize


    \author HigePon
    \date   create:2003/03/08 update:
*/
#define PIT_MODE   0x43
#define PIT_COUNT0 0x40
void pic_init() {

    /* master */
    outportb(0x20, 0x11); /* ICW1 */
    outportb(0x21, 0x60); /* ICW2 */ /* IRQ 0h = int 60h */
    outportb(0x21, 0x04); /* ICW3 */
    outportb(0x21, 0x01); /* ICW4 */

    /* slave */
    outportb(0xa0, 0x11); /* ICW1 */
    outportb(0xa1, 0x28); /* ICW2 */ /* IRQ  8h = int 28h */
    outportb(0xa1, 0x02); /* ICW3 */
    outportb(0xa1, 0x01); /* ICW4 */

    /* timer settings 10ms */
    //    setTimerInterval(10);
    /* timer settings 10ms */
    dword timer_count = 10 * 1193181 / 1000;
    outportb(PIT_MODE, 0x36);
    outportb(PIT_COUNT0, timer_count & 0xff);
    outportb(PIT_COUNT0, timer_count >> 8);

    /* mask all interrupt */
    outportb(0x21, 0xff);
    outportb(0xA1, 0xff);

    /* enable key board */
    outportb(0x21, inportb(0x21) & 0x3f);

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
    outportb(PIT_MODE, 0x34);
    outportb(PIT_COUNT0, timerCounter & 0xff);
    outportb(PIT_COUNT0, timerCounter >> 8);
}

/*!
    \brief disable timer

    disable timer interrupt
    mask IMR bit 0

    \author HigePon
    \date   create:2003/01/12 update:
*/
void disableTimer() {

    outportb(0x21, inportb(0x21) | 0x01);
}

/*!
    \brief enable timer

    enable timer interrupt

    \author HigePon
    \date   create:2003/01/12 update:
*/
void enableTimer() {

    outportb(0x21, inportb(0x21) & 0xFE);
}

/*!
    \brief disable keyboard interrupt

    \author HigePon
    \date   create:2003/03/08 update:
*/
void disableKeyboard() {

    outportb(0x21, inportb(0x21) | 0x02);
}

/*!
    \brief enable keyboard interrupt

    \author HigePon
    \date   create:2003/03/08 update:
*/
void enableKeyboard() {

    outportb(0x21, inportb(0x21) & 0xFD);
}
