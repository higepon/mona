/*!
    \file  pic.cpp
    \brief pic functions

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

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
void pic_init() {

    /* master */
    outportb(0x20, 0x11); /* ICW1 */
    outportb(0x21, 0x00); /* ICW2 */
    outportb(0x21, 0x04); /* ICW3 */
    outportb(0x21, 0x01); /* ICW4 */

    /* slave */
    outportb(0xa0, 0x11); /* ICW1 */
    outportb(0xa1, 0x28); /* ICW2 */
    outportb(0xa1, 0x02); /* ICW3 */
    outportb(0xa1, 0x01); /* ICW4 */

    /* mask all interrupt */
    outportb(0x21, 0xff);
    outportb(0xA1, 0xff);

    /* enable key board */
    outportb(0x21, inportb(0x21) & 0x3f);

    return;
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
