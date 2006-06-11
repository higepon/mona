/*!
    \file  FileSystemEntry.h
    \brief FileSystemEntry

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2005/01/23 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _FILE_SYSTEM_ENTRY_
#define _FILE_SYSTEM_ENTRY_

#ifdef MONA
#include <monapi.h>
#include <monapi/CString.h>
#else
#ifdef ON_LINUX
#include "types.h"
#include "monapi/CString.h"
#else
#include "types.h"
#include "CString.h"
#endif
#endif


#include "FileDate.h"

/*----------------------------------------------------------------------
    FileSystemEntry
----------------------------------------------------------------------*/
class FileSystemEntry
{
public:
    FileSystemEntry() {}
    virtual ~FileSystemEntry() {}

public:
    virtual bool IsDirectory()          = 0;
    virtual dword GetSize()             = 0;
    virtual MonAPI::CString GetName()   = 0;
    virtual FileDate* GetCreateDate()   = 0;
    virtual FileDate* GetModifiedDate() = 0;
};

#endif
