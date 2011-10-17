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


#include <sys/Bitmap.h>
#if 0
#define AVL_ASSERT(x) if (!x) { if (g_log) g_log->printf("ASSERT failure %s %s:%d\n", #x, __FILE__, __LINE__); }
#define HLIST_ASSERT(x) if (!x) { if (g_log) g_log->printf("ASSERT failure %s %s:%d\n", #x, __FILE__, __LINE__); }
#endif
#include <sys/SymbolDictionary.h>
#include <sys/KStat.h>

typedef uint32_t PageEntry;
typedef uint32_t LinearAddress;
typedef uint32_t PhysicalAddress;
class SharedMemoryObject;

class PageManager {

  public:
    PageManager(uintptr_t systemMemorySizeByte, PhysicalAddress vramAddress, uintptr_t vramSizeByte);

  public:
    void flushPageCache() const;
    void getPagePoolInfo(uint32_t* freeNum, uint32_t* totalNum, uint32_t* pageSize);

    void returnPhysicalPage(PhysicalAddress address);
    void returnPhysicalPages(PageEntry* directory);
    void returnPageTable(PageEntry* table);
    uint8_t* allocateDMAMemory(PageEntry* directory, int size, bool isUser);
    void deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size);

    intptr_t allocateContiguous(PageEntry* directory, LinearAddress laddress, int pageNum);
    intptr_t deallocateContiguous(PageEntry* directory, LinearAddress address, int PageNum);


    void mapOnePageByPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress, bool isWritable, bool isUser)
    {
        PageEntry* table = getOrAllocateTable(directory, laddress, isWritable, isUser);
        uint32_t tableIndex = getTableIndex(laddress);
        setAttribute(&(table[tableIndex]), PAGE_PRESENT, isWritable, isUser, paddress);
    }

    int mapOnePage(PageEntry* directory, PhysicalAddress& mappedAddres, LinearAddress laddress, bool isWritable, bool isUser);
    void unmapOnePage(PageEntry* directory, LinearAddress laddress, bool returnPages = true);
    void unmapPages(PageEntry* directory, LinearAddress start, int pageNum, bool returnPages = true);
    void unmapRange(PageEntry* directory, LinearAddress start, LinearAddress end, bool returnPages = true);

    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address)
    {
        PageEntry value =  (address & 0xfffff000) | (present ? ARCH_PAGE_PRESENT : 0x00)
            | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);
        (*entry) = value;
        return true;
    }

    bool setAttribute(PageEntry* entry, bool present, bool writable, bool isUser);
    bool setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser);

    bool getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress);




    void startPaging(PhysicalAddress address);
    void stopPaging();
    PageEntry* createPageDirectory();
    bool pageFaultHandler(ThreadInfo* thread, LinearAddress address, uint32_t error);
    inline static bool isPresent(PageEntry entry)
    {

        return entry & ARCH_PAGE_PRESENT;
    }

    void showCurrentStackTrace();
    bool enableStackTrace(uint32_t pid, uint8_t *data, uint32_t size);
    void disableStackTrace(uint32_t pid);

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

    PageEntry* getTable(PageEntry* directory, LinearAddress laddress)
    {
        uint32_t directoryIndex = getDirectoryIndex(laddress);
        if (isPresent(directory[directoryIndex])) {
            return getTableAt(directory, directoryIndex);
        } else {
            return NULL;
        }
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
        MONA_ASSERT((paddress % ARCH_PAGE_SIZE) == 0);
        MONA_ASSERT((laddress % ARCH_PAGE_SIZE) == 0);
        PageEntry* table = getOrAllocateTable(directory, laddress, isWritable, isUser);
        uint32_t tableIndex = getTableIndex(laddress);
        setAttribute(&(table[tableIndex]), true, isWritable, isUser, paddress);
    }

    void showPageFault(ThreadInfo* threadInfo, uint32_t address);

  private:
    uintptr_t systemMemorySizeByte_;
    Bitmap* memoryMap_;
    Bitmap* pageTablePool_;
    PhysicalAddress pageTablePoolAddress_;
    PhysicalAddress vramAddress_;
    uintptr_t vramSizeByte_;
    PageEntry* kernelDirectory_;
    Bitmap* reservedDMAMap_;
    SymbolDictionary::SymbolDictionaryMap symbolDictionaryMap_;
    void setPageDirectory(PhysicalAddress address);

  public:
    enum {
        PAGE_TABLE_POOL_SIZE_BYTE   = 2 * 1024 * 1024,
        KERNEL_RESERVED_REGION_END = 0x1600000,
        DMA_RESERVED_REGION_START  = KERNEL_RESERVED_REGION_END,
        DMA_RESERVED_REGION_SIZE_BYTE = 1 * 1024 * 1024,
        SHARED_MEMORY_REGION_START = 0x90000000,
        SHARED_MEMORY_REGION_END   = 0x9FFFFFFF
    };

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
