/*!
    \file  info.h
    \brief information function

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/25 update:$Date$
*/
#ifndef _MONA_INFO_
#define _MONA_INFO_

#define ERROR       10
#define WARNING     20
#define DEV_ERROR   30
#define DEV_WARNING 40
#define DEV_NOTICE  50
#define DEBUG       60
#define DUMP        70

void info(int level, const char *format, ...);

#endif
