/*!
    \file  info.h
    \brief information function

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/25 update:$Date$
*/

#ifndef __INFO_H__
#define __INFO_H__

#define INFO_MSG          5
#define INFO_ERROR       10
#define INFO_WARNING     20
#define INFO_DEV_ERROR   30
#define INFO_DEV_WARNING 40
#define INFO_DEV_NOTICE  50
#define INFO_DEBUG       60
#define INFO_DUMP        70

void info(int level, const char *format, ...);

#endif
