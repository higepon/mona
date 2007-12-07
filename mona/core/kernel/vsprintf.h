/*!
    \file  vsprintf.h
    \brief vsprintf

    Copyright (c) 2007 shadow, HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision: 3946 $
    \date   create:2007/12/08 update:$Date$
*/
#ifndef _MONA_VSPRINTF_
#define _MONA_VSPRINTF_

#include "stdarg.h"

int vsprintf(char *s, const char *format, va_list arg);

#endif
