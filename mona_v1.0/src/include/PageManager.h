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

#define PAGE_PRESENT   0x01
#define PAGE_RW        0x02
#define PAGE_READ_ONLY 0x00
#define PAGE_USER      0x04
#define PAGE_KERNEL    0x00

#define PAGE_SIZE      4096
#define PAGE_TABLE_NUM 1024

typedef dword PageEntry;
typedef dword LinearAddress;
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
    bool allocatePhysicalPage(PageEntry* directory, LinearAddress address, byte rw, byte user);

    void setPageDirectory(PhysicalAddress address);
    void startPaging();
    void stopPaging();
    PageEntry* createNewPageDirectory();
    bool pageFaultHandler(LinearAddress address, dword error);

  private:
    PageDirectory* allocatePageDirectory();
    void makePageEntry(PageEntry* entry, PhysicalAddress address, byte present, byte rw, byte user) const;
    PageEntry* allocatePageTable() const;

    inline bool isPresent(PageEntry* entry) const {

        return (*entry) & PAGE_PRESENT;
    }


  private:
    BitMap*       memoryMap_;
    PageDirectory pageDirectory_;

};

class Segment {

  public:
    virtual bool faultHandler(LinearAddress address, dword error) = 0;
    virtual int getErrorNumber() {return errorNumber_;}

  protected:
    LinearAddress start_;
    dword         size_;
    byte errorNumber_;
};

class Stack : public Segment {

  public:
    Stack(LinearAddress start, dword size);
    Stack(LinearAddress start, dword initileSize, dword maxSize);
    virtual ~Stack();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);

  private:
    bool tryExtend(LinearAddress address);
    bool allocatePage(LinearAddress address);

  protected:
    bool isAutoExtend_;
    dword maxSize_;

};



#endif
