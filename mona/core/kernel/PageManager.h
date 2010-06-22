/*!
    \file  PageManager.h
    \brief some functions for page management

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/08/19 update:$Date$
*/
#ifndef _MONA_PAGE_MANAGER_
#define _MONA_PAGE_MANAGER_


#include "BitMap.h"
#include <sys/SymbolDictionary.h>

typedef uint32_t PageEntry;
typedef uint32_t LinearAddress;
typedef uint32_t PhysicalAddress;

class PageManager {

  public:
    PageManager(uintptr_t systemMemorySizeByte, PhysicalAddress vramAddress, uintptr_t vramSizeByte);

  public:
    void flushPageCache() const;
    void getPagePoolInfo(uint32_t* freeNum, uint32_t* totalNum, uint32_t* pageSize);

    void returnPhysicalPage(PhysicalAddress address);
    void returnPhysicalPages(PageEntry* directory);
    void returnPageTable(PageEntry* table);
    void returnPages(PageEntry* directory, LinearAddress address, uint32_t size);
    uint8_t* allocateDMAMemory(PageEntry* directory, int size, bool isUser);
    void deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size);

    intptr_t allocateContiguous(PageEntry* directory, LinearAddress laddress, int pageNum);
    void deallocateContiguous(PageEntry* directory, LinearAddress address, int PageNum);


    int mapOnePageByPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress, bool isWritable, bool isUser);
    int mapOnePage(PageEntry* directory, LinearAddress laddress, bool isWritable, bool isUser);

    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address);
    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser);
    bool setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser);

    bool getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress);

    void setAbsent(PageEntry* directory, LinearAddress start, uint32_t size);


    void startPaging(PhysicalAddress address);
    void stopPaging();
    PageEntry* createPageDirectory();
    bool pageFaultHandler(LinearAddress address, uint32_t error, uint32_t eip);
    inline static bool isPresent(PageEntry entry)
    {

        return entry & ARCH_PAGE_PRESENT;
    }

    bool enableStackTrace(uint32_t pid, uint8_t *data, uint32_t size) {
        SymbolDictionary::SymbolDictionary* dict = new SymbolDictionary::SymbolDictionary();
        if(!dict->deserialize(data, size)) // deserialize use many memory, so I check error only here.
        {
            delete dict;
            return false;
        }
        symbolDictionaryMap_.add(pid, dict);
        return true;
    }
    void disableStackTrace(uint32_t pid) {
        symbolDictionaryMap_.remove(pid);
    }

    void showCurrentStackTrace();

  public:
    inline static int getDirectoryIndex(LinearAddress address) {

        return (address >> 22);
    }

    inline static int getTableIndex(LinearAddress address) {

        return ((address >> 12) & 0x3FF);
    }

  private:
    PageEntry* allocatePageTable() const;

    void initializePagePool();
    void initializePageTablePool(uintptr_t poolSizeByte);

    PhysicalAddress align4Kb(PhysicalAddress address)
    {
        return ((address + 4095) & 0xFFFFF000);
    }

    uintptr_t bytesToPageNumber(uintptr_t sizeInBytes)
    {
        return (sizeInBytes + ARCH_PAGE_SIZE - 1) / ARCH_PAGE_SIZE;
    }

    PageEntry* getTableAt(PageEntry* directory, int directoryIndex) const
    {
        return (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    }

    PageEntry* getOrAllocateTable(PageEntry* directory, LinearAddress laddress, bool isWritable, bool isUser)
    {
        uint32_t directoryIndex = getDirectoryIndex(laddress);
        PageEntry* table;
        if (isPresent(directory[directoryIndex])) {
            table = getTableAt(directory, directoryIndex);
        } else {
            table = allocatePageTable();
            setAttribute(&(directory[directoryIndex]), true, isWritable, isUser, (PhysicalAddress)table);
        }
        return table;
    }

    void mapAsLinearEqPhysical(PageEntry* directory, LinearAddress address, bool isWritable, bool isUser)
    {
        map(directory, address, address, isWritable, isUser);
    }

    void map(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress, bool isWritable, bool isUser)
    {
        ASSERT((paddress % ARCH_PAGE_SIZE) == 0);
        ASSERT((laddress % ARCH_PAGE_SIZE) == 0);
        PageEntry* table = getOrAllocateTable(directory, laddress, isWritable, isUser);
        uint32_t tableIndex = getTableIndex(laddress);
        setAttribute(&(table[tableIndex]), true, isWritable, isUser, paddress);
    }

  private:
    uintptr_t systemMemorySizeByte_;
    BitMap* memoryMap_;
    BitMap* pageTablePool_;
    PhysicalAddress pageTablePoolAddress_;
    PhysicalAddress vramAddress_;
    uintptr_t vramSizeByte_;
    PageEntry* kernelDirectory_;
    BitMap* reservedDMAMap_;
    SymbolDictionary::SymbolDictionaryMap symbolDictionaryMap_; 
    void setPageDirectory(PhysicalAddress address);
    enum {
        PAGE_TABLE_POOL_SIZE_BYTE   = 2 * 1024 * 1024,
        KERNEL_RESERVED_REGION_END = 0xC00000,
        DMA_RESERVED_REGION_START  = KERNEL_RESERVED_REGION_END,
        DMA_RESERVED_REGION_SIZE_BYTE = 1 * 1024 * 1024,
        SHARED_MEMORY_REGION_START = 0x90000000,
        SHARED_MEMORY_REGION_END   = 0x9FFFFFFF
    };

  public:

    enum {
        PAGE_WRITABLE  = true,
        PAGE_READ_ONLY = false,
        PAGE_USER      = true,
        PAGE_PRESENT   = true,
        PAGE_ABSENT    = false,
        PAGE_KERNEL    = false,
        forbidden_comma
    };

    enum {
        FAULT_NOT_EXIST          = 0x01,
        FAULT_NOT_WRITABLE       = 0x02,
        forbidden_comma2
    };

    enum {
        ARCH_FAULT_NOT_EXIST     = 0x00,
        ARCH_FAULT_ACCESS_DENIED = 0x01,
        ARCH_FAULT_READ          = 0x00,
        ARCH_FAULT_WRITE         = 0x02,
        ARCH_FAULT_WHEN_KERNEL   = 0x00,
        ARCH_FAULT_WHEN_USER     = 0x04,
        ARCH_PAGE_PRESENT        = 0x01,
        ARCH_PAGE_RW             = 0x02,
        ARCH_PAGE_READ_ONLY      = 0x00,
        ARCH_PAGE_USER           = 0x04,
        ARCH_PAGE_KERNEL         = 0x00,
        ARCH_PAGE_SIZE           = 4096,
        ARCH_PAGE_TABLE_NUM      = 1024,
        forbidden_comma3
    };
};

#endif
