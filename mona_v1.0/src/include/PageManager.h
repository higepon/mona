/*!
    \file  PageManager.h
    \brief some functions for page management

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/08/19 update:$Date$
*/
#ifndef _MONA_PAGE_MANAGER_
#define _MONA_PAGE_MANAGER_

#include <Queue.h>
#include <BitMap.h>

#define PAGE_PRESENT 0x01
#define PAGE_RW      0x02
#define PAGE_USER    0x04

#define PAGE_TABLE_NUM 1024

typedef dword PageEntry;
typedef dword LenearAddress;
typedef dword PhysicalAddress;


class PageDirectory : public Queue {

    PageEntry* pageEntry_;
};

class PageManager {

  public:

    PageManager(dword totalMemorySize);

  public:
    void setup();
    void flushPageCache() const;

    bool allocatePhysicalPage(PageEntry* pageEntry);

    void setPageDirectory(PhysicalAddress address);
    void startPaging();
    void stopPaging();

  private:
    PageDirectory* allocatePageDirectory();
    void makePageEntry(PageEntry* entry, PhysicalAddress address, byte present, byte rw, byte user) const;
    PageEntry* allocatePageTable() const;

  private:
    BitMap*       memoryMap_;
    PageDirectory pageDirectory_;

};


#endif
