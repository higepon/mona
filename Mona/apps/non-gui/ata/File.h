/*!
    \file  File.h
    \brief File

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/11/06 update:$Date$
*/

#ifndef _MONA_FILE_
#define _MONA_FILE_

#include "sys/types.h"

/*----------------------------------------------------------------------
    File
----------------------------------------------------------------------*/
class File
{
public:
    virtual dword Read(void* buffer, dword size) = 0;
    virtual bool Seek(int position, int flag)    = 0;
    virtual dword GetSize() const                = 0;
    virtual const char* GetName()                = 0;

public:
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

#endif
