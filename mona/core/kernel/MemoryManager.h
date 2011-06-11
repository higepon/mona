/*!
    \file  MemoryManager.h
    \brief class MemoryManager

    class MemoryManager

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/09/28 update:$Date$
*/
#ifndef _MONA_MEMORY_MANAGER_
#define _MONA_MEMORY_MANAGER_

#include <sys/types.h>

class MemoryManager
{
  public:
    MemoryManager();
    ~MemoryManager();

  public:
    void initialize(uint32_t size, uint32_t end);
    void free(void* address);
    void* allocate(uint32_t size);
    uint32_t getFreeMemorySize() const;
    uint32_t getUsedMemorySize() const;
    static uint32_t getPhysicalMemorySize();
    void debugprint();

  private:
    typedef struct Header
    {
        struct Header* next;
        uint32_t size;
        uint32_t magic;
        uint32_t padding;
    };

    void setNext(Header* p, Header* next);

    Header* freeList;
    uint32_t start;
    uint32_t end;
};

#endif
