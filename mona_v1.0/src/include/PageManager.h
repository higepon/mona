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
    inline bool setAttribute(PageEntry* entry, bool present, bool writable, bool kernel);
    inline bool setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool kernel);
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

        return (*entry) & ARCH_PAGE_PRESENT;
    }

  private:
    BitMap*       memoryMap_;
    PageDirectory pageDirectory_;

  public:
    static const byte FAULT_NOT_EXIST          = 0x01;
    static const byte FAULT_NOT_WRITABLE       = 0x02;

    static const byte ARCH_FAULT_NOT_EXIST     = 0x00;
    static const byte ARCH_FAULT_ACCESS_DENIED = 0x01;
    static const byte ARCH_FAULT_READ          = 0x00;
    static const byte ARCH_FAULT_WRITE         = 0x02;
    static const byte ARCH_FAULT_WHEN_KERNEL   = 0x00;
    static const byte ARCH_FAULT_WHEN_USER     = 0x04;
    static const byte ARCH_PAGE_PRESENT        = 0x01;
    static const byte ARCH_PAGE_RW             = 0x02;
    static const byte ARCH_PAGE_READ_ONLY      = 0x00;
    static const byte ARCH_PAGE_USER           = 0x04;
    static const byte ARCH_PAGE_KERNEL         = 0x00;
    static const int  ARCH_PAGE_SIZE           = 4096;
    static const int  ARCH_PAGE_TABLE_NUM      = 1024;
};

class Segment {

  public:
    virtual bool faultHandler(LinearAddress address, dword error) = 0;
    virtual int getErrorNumber() {return errorNumber_;}
    virtual LinearAddress getStart() {return start_;}
    virtual dword getSize() {return size_;}

  protected:
    LinearAddress start_;
    dword         size_;
    byte errorNumber_;

  public:
    static const byte FAULT_STACK_OVERFLOW = 0x01;
    static const byte FAULT_OUT_OF_RANGE   = 0x02;
    static const byte FAULT_UNKNOWN        = 0x03;
};

class StackSegment : public Segment {

  public:
    StackSegment(LinearAddress start, dword size);
    StackSegment(LinearAddress start, dword initileSize, dword maxSize);
    virtual ~StackSegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);

  private:
    bool tryExtend(LinearAddress address);
    bool allocatePage(LinearAddress address);

  protected:
    bool isAutoExtend_;
    dword maxSize_;

};

class HeapSegment : public Segment {

  public:
    HeapSegment(LinearAddress start, dword size);
    virtual ~HeapSegment();

  public:
    virtual bool faultHandler(LinearAddress address, dword error);
};

#endif
