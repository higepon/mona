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

#include "BitMap.h"

typedef dword PageEntry;
typedef dword LinearAddress;
typedef dword PhysicalAddress;

class PageManager {

  public:
    PageManager(dword totalMemorySize);

  public:
    void setup(PhysicalAddress vram);
    void flushPageCache() const;
    void getPagePoolInfo(dword* freeNum, dword* totalNum, dword* pageSize);

    void returnPhysicalPage(PhysicalAddress address);
    void returnPhysicalPages(PageEntry* directory);
    void returnPageTable(PageEntry* table);
    void returnPages(PageEntry* directory, LinearAddress address, dword size);
    byte* allocateDMAMemory(PageEntry* directory, int size, bool isUser);
    void deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size);

    int allocatePhysicalPage(PageEntry* pageEntry, bool present, bool writable, bool isUser) const;
    int allocatePhysicalPage(PageEntry* pageEntry, bool present, bool writable, bool isUser, PhysicalAddress address) const;
    int allocatePhysicalPage(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress, bool present, bool writable, bool isUser) const;
    int allocatePhysicalPage(PageEntry* directory, LinearAddress laddress, bool present, bool writable, bool isUser) const;

    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address) const;
    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser) const;
    bool setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser) const;

    bool getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress);

    void setAbsent(PageEntry* directory, LinearAddress start, dword size) const;

    void setPageDirectory(PhysicalAddress address);
    void startPaging();
    void stopPaging();
    PageEntry* createNewPageDirectory();
    PageEntry* createKernelPageDirectory();
    bool pageFaultHandler(LinearAddress address, dword error, dword eip);
    inline static bool isPresent(PageEntry* entry) {

        return (*entry) & ARCH_PAGE_PRESENT;
    }

  public:
    inline static int getDirectoryIndex(LinearAddress address) {

        return (address >> 22);
    }

    inline static int getTableIndex(LinearAddress address) {

        return ((address >> 12) & 0x3FF);
    }

  private:
    PageEntry* allocatePageTable() const;

  private:
    BitMap* memoryMap_;
    BitMap* pageTablePool_;
    PhysicalAddress pageTablePoolAddress_;
    PhysicalAddress vram_;
    PageEntry* kernelDirectory_;
    BitMap* reservedDMAMap_;

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
    static const int  PAGE_TABLE_POOL_SIZE     = 1 * 1024 * 1024; // 1MB
};

#endif
