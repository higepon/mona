/*!
    \file  MemoryManager.h
    \brief class MemoryManager

    class MemoryManager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/28 update:$Date$
*/
#ifndef _MONA_MEMORY_MANAGER_
#define _MONA_MEMORY_MANAGER_

#include <sys/types.h>

namespace MonAPI {

class MemoryManager
{
  public:
    MemoryManager();
    ~MemoryManager();

  public:
    void initialize(dword size, dword end);
    void free(void* address);
    void* allocate(dword size);
    dword getFreeMemorySize() const;
    dword getUsedMemorySize() const;
    static dword getPhysicalMemorySize();
    void debugprint();

  private:
    typedef struct Header
    {
        struct Header* next;
        dword size;
        dword magic;
        dword padding;
    };

    void setNext(Header* p, Header* next);

    Header* freeList;
    dword start;
    dword end;
};

}

#endif
