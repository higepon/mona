/*!
    \file  pic.h
    \brief pic functions

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/08 update:$Date$
*/

#ifndef _MONA_PIC_
#define _MONA_PIC_

#include<io.h>

void pic_init();
void setTimerInterval(dword ms);
void enableTimer();
void disableTimer();
void enableKeyboard();
void disableKeyboard();

#endif
