/*!
    \file   monaIhandler.cpp
    \brief  interrupt handlers

     interrupt handlers

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/25 update:$Date$
*/

#include<monaIdt.h>
#include<monaVga.h>
#include<monaIo.h>
#include<KeyBoardManager.h>
#include<FDCDriver.h>
#include<ProcessManager.h>
#include<monaTypes.h>
#include<monaTester.h>
#include<monaKernel.h>
#include<string>

/*!
    \brief key stroke handler

    key storoke handler IRQ 1

    \author HigePon
    \date   create:2002/07/25 update:2002/10/12
*/
void keyStrokeHandler() {

    pusha();

    /* get scancode */
    unsigned char scancode = inportb(0x60);

    /* set key scan code */
    KeyBoardManager& km = KeyBoardManager::instance();
    km.setKeyScanCode(scancode);

    /* EOI is below for IRQ 0-7 */
    outportb(0x20, 0x20);

    popa();

    /* iret */
    iret();

    return;
}

/*!
    \brief fault0d handler

    fault0d handler

    \author HigePon
    \date   create:2002/09/06 update:2002/12/28
*/
void fault0dHandler() {

    _sysdumpReg("fault0d", false, false);
    _sysdumpStack();
    panic("fault0d");
}


/*!
    \brief dummy handler

    dummy handler

    \author HigePon
    \date   create:2002/07/25 update:2002/07/27
*/
void dummy() {

    _sysPrint("dummy");

    pusha();

    /* EOI is below for IRQ 8-15 */
    outportb(0xA0, 0x20);
    outportb(0x20, 0x20);

    popa();

    iret();
    return;
}

/*!
    \brief floppy disk controller's interrupt handler

    floppy disk controller's interrupt handler

    \author syn
    \date create:2002/10/14 update:2002/10/14
*/
void fdcHandler(){
    gFDCDriver1->interruptHandler();
    outportb(0x20, 0x20);

    /* iret */
    iret();
}

/*!
    \brief timer handler

    timer handler
    at this function task switch occurs.

    \author HigePon
    \date   create:2002/11/21 update:2003/01/15
*/
void timerHandler() {

    static dword idx = 0;
    idx++;

    /* EOI is below for IRQ 8-15 */
    outportb(0xA0, 0x20);
    outportb(0x20, 0x20);
    //    iret();

    if (idx < 200) {
        iret();
    } else if (idx > 0xf000000) {
        idx = 1;
    } else if (idx % 20 != 0) iret();

    /* determine next process or thread and run it */
    ProcessManager& pm = ProcessManager::instance();
    pm.schedule();

    /* do nothing */
    if (pm.current == pm.next) iret();

    _switchProcess(pm.current, pm.next);

    return;
}

/*! global handler list */
handler_st handlers[HANDLER_NUM] = {
     {0x00, &dummy}
   , {0x01, &dummy}
   , {0x02, &dummy}
   , {0x03, &dummy}
   , {0x04, &dummy}
   , {0x05, &dummy}
   , {0x06, &dummy}
   , {0x07, &dummy}
   , {0x08, &timerHandler}
   , {0x09, &keyStrokeHandler}
   , {0x0A, &dummy}
   , {0x0B, &dummy}
   , {0x0C, &dummy}
   , {0x0D, &fault0dHandler}
   , {0x0E, &dummy}
   , {0x0F, &dummy}
   , {0x10, &dummy}
   , {0x11, &dummy}
   , {0x12, &dummy}
   , {0x13, &dummy}
   , {0x14, &dummy}
   , {0x15, &dummy}
   , {0x16, &dummy}
   , {0x17, &dummy}
   , {0x18, &dummy}
   , {0x19, &dummy}
   , {0x1A, &dummy}
   , {0x1B, &dummy}
   , {0x1C, &dummy}
   , {0x1D, &dummy}
   , {0x1E, &dummy}
   , {0x1F, &dummy}
   , {0x20, &dummy}
   , {0x21, &dummy}
   , {0x22, &dummy}
   , {0x23, &dummy}
   , {0x24, &dummy}
   , {0x25, &dummy}
   , {0x26, &dummy}
   , {0x27, &dummy}
   , {0x28, &dummy}
   , {0x29, &dummy}
   , {0x2A, &dummy}
   , {0x2B, &dummy}
   , {0x2C, &dummy}
   , {0x2D, &dummy}
   , {0x2E, &dummy}
   , {0x2F, &dummy}
   , {0x30, &dummy}
   , {0x31, &dummy}
   , {0x32, &dummy}
   , {0x33, &dummy}
   , {0x34, &dummy}
   , {0x35, &dummy}
   , {0x36, &dummy}
   , {0x37, &dummy}
   , {0x38, &dummy}
   , {0x39, &dummy}
   , {0x3A, &dummy}
   , {0x3B, &dummy}
   , {0x3C, &dummy}
   , {0x3D, &dummy}
   , {0x3E, &dummy}
   , {0x3F, &dummy}
   , {0x40, &dummy}
   , {0x41, &dummy}
   , {0x42, &dummy}
   , {0x43, &dummy}
   , {0x44, &dummy}
   , {0x45, &dummy}
   , {0x46, &dummy}
   , {0x47, &dummy}
   , {0x48, &dummy}
   , {0x49, &dummy}
   , {0x4A, &dummy}
   , {0x4B, &dummy}
   , {0x4C, &dummy}
   , {0x4D, &dummy}
   , {0x4E, &dummy}
   , {0x4F, &dummy}
   , {0x50, &dummy}
   , {0x51, &dummy}
   , {0x52, &dummy}
   , {0x53, &dummy}
   , {0x54, &dummy}
   , {0x55, &dummy}
   , {0x56, &dummy}
   , {0x57, &dummy}
   , {0x58, &dummy}
   , {0x59, &dummy}
   , {0x5A, &dummy}
   , {0x5B, &dummy}
   , {0x5C, &dummy}
   , {0x5D, &dummy}
   , {0x5E, &dummy}
   , {0x5F, &dummy}
   , {0x60, &dummy}
   , {0x61, &dummy}
   , {0x62, &dummy}
   , {0x63, &dummy}
   , {0x64, &dummy}
   , {0x65, &dummy}
   , {0x66, &dummy}
   , {0x67, &dummy}
   , {0x68, &dummy}
   , {0x69, &dummy}
   , {0x6A, &dummy}
   , {0x6B, &dummy}
   , {0x6C, &dummy}
   , {0x6D, &dummy}
   , {0x6E, &dummy}
   , {0x6F, &dummy}
   , {0x70, &dummy}
   , {0x71, &dummy}
   , {0x72, &dummy}
   , {0x73, &dummy}
   , {0x74, &dummy}
   , {0x75, &dummy}
   , {0x76, &dummy}
   , {0x77, &dummy}
   , {0x78, &dummy}
   , {0x79, &dummy}
   , {0x7A, &dummy}
   , {0x7B, &dummy}
   , {0x7C, &dummy}
   , {0x7D, &dummy}
   , {0x7E, &dummy}
   , {0x7F, &dummy}
   , {0x80, &dummy}
   , {0x81, &dummy}
   , {0x82, &dummy}
   , {0x83, &dummy}
   , {0x84, &dummy}
   , {0x85, &dummy}
   , {0x86, &dummy}
   , {0x87, &dummy}
   , {0x88, &dummy}
   , {0x89, &dummy}
   , {0x8A, &dummy}
   , {0x8B, &dummy}
   , {0x8C, &dummy}
   , {0x8D, &dummy}
   , {0x8E, &dummy}
   , {0x8F, &dummy}
   , {0x90, &dummy}
   , {0x91, &dummy}
   , {0x92, &dummy}
   , {0x93, &dummy}
   , {0x94, &dummy}
   , {0x95, &dummy}
   , {0x96, &dummy}
   , {0x97, &dummy}
   , {0x98, &dummy}
   , {0x99, &dummy}
   , {0x9A, &dummy}
   , {0x9B, &dummy}
   , {0x9C, &dummy}
   , {0x9D, &dummy}
   , {0x9E, &dummy}
   , {0x9F, &dummy}
   , {0xA0, &dummy}
   , {0xA1, &dummy}
   , {0xA2, &dummy}
   , {0xA3, &dummy}
   , {0xA4, &dummy}
   , {0xA5, &dummy}
   , {0xA6, &dummy}
   , {0xA7, &dummy}
   , {0xA8, &dummy}
   , {0xA9, &dummy}
   , {0xAA, &dummy}
   , {0xAB, &dummy}
   , {0xAC, &dummy}
   , {0xAD, &dummy}
   , {0xAE, &dummy}
   , {0xAF, &dummy}
   , {0xB0, &dummy}
   , {0xB1, &dummy}
   , {0xB2, &dummy}
   , {0xB3, &dummy}
   , {0xB4, &dummy}
   , {0xB5, &dummy}
   , {0xB6, &dummy}
   , {0xB7, &dummy}
   , {0xB8, &dummy}
   , {0xB9, &dummy}
   , {0xBA, &dummy}
   , {0xBB, &dummy}
   , {0xBC, &dummy}
   , {0xBD, &dummy}
   , {0xBE, &dummy}
   , {0xBF, &dummy}
   , {0xC0, &dummy}
   , {0xC1, &dummy}
   , {0xC2, &dummy}
   , {0xC3, &dummy}
   , {0xC4, &dummy}
   , {0xC5, &dummy}
   , {0xC6, &dummy}
   , {0xC7, &dummy}
   , {0xC8, &dummy}
   , {0xC9, &dummy}
   , {0xCA, &dummy}
   , {0xCB, &dummy}
   , {0xCC, &dummy}
   , {0xCD, &dummy}
   , {0xCE, &dummy}
   , {0xCF, &dummy}
   , {0xD0, &dummy}
   , {0xD1, &dummy}
   , {0xD2, &dummy}
   , {0xD3, &dummy}
   , {0xD4, &dummy}
   , {0xD5, &dummy}
   , {0xD6, &dummy}
   , {0xD7, &dummy}
   , {0xD8, &dummy}
   , {0xD9, &dummy}
   , {0xDA, &dummy}
   , {0xDB, &dummy}
   , {0xDC, &dummy}
   , {0xDD, &dummy}
   , {0xDE, &dummy}
   , {0xDF, &dummy}
   , {0xE0, &dummy}
   , {0xE1, &dummy}
   , {0xE2, &dummy}
   , {0xE3, &dummy}
   , {0xE4, &dummy}
   , {0xE5, &dummy}
   , {0xE6, &dummy}
   , {0xE7, &dummy}
   , {0xE8, &dummy}
   , {0xE9, &dummy}
   , {0xEA, &dummy}
   , {0xEB, &dummy}
   , {0xEC, &dummy}
   , {0xED, &dummy}
   , {0xEE, &dummy}
   , {0xEF, &dummy}
   , {0xF0, &dummy}
   , {0xF1, &dummy}
   , {0xF2, &dummy}
   , {0xF3, &dummy}
   , {0xF4, &dummy}
   , {0xF5, &dummy}
   , {0xF6, &dummy}
   , {0xF7, &dummy}
   , {0xF8, &dummy}
   , {0xF9, &dummy}
   , {0xFA, &dummy}
   , {0xFB, &dummy}
   , {0xFC, &dummy}
   , {0xFD, &dummy}
   , {0xFE, &dummy}
   , {0xFF, &dummy}
};
