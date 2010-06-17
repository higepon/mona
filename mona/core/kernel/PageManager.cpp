
/*!
  \file  PageManager.cpp
  \brief class PageManager

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2003/08/23 update:$Date$
*/

#include <sys/types.h>
#include "global.h"
#include "PageManager.h"
#include "string.h"
#include "operator.h"
#include "Segments.h"
#include "Uart.h"
#include "ihandlers.h"


/* independent from architecture */
const uint8_t PageManager::FAULT_NOT_EXIST;
const uint8_t PageManager::FAULT_NOT_WRITABLE;

/* depend on architecture */
const uint8_t PageManager::ARCH_FAULT_NOT_EXIST;
const uint8_t PageManager::ARCH_FAULT_ACCESS_DENIED;
const uint8_t PageManager::ARCH_FAULT_READ;
const uint8_t PageManager::ARCH_FAULT_WRITE;
const uint8_t PageManager::ARCH_FAULT_WHEN_KERNEL;
const uint8_t PageManager::ARCH_FAULT_WHEN_USER;
const uint8_t PageManager::ARCH_PAGE_PRESENT;
const uint8_t PageManager::ARCH_PAGE_RW;
const uint8_t PageManager::ARCH_PAGE_READ_ONLY;
const uint8_t PageManager::ARCH_PAGE_USER;
const uint8_t PageManager::ARCH_PAGE_KERNEL;
const int  PageManager::ARCH_PAGE_SIZE;
const int  PageManager::ARCH_PAGE_TABLE_NUM;
const int  PageManager::PAGE_TABLE_POOL_SIZE;

PageManager::PageManager(uint32_t totalMemorySize, PhysicalAddress vramAddress, int vramSizeByte)
  : vramAddress_(vramAddress), vramSizeByte_(vramSizeByte)
{
    initializePageTablePool(PAGE_TABLE_POOL_SIZE);
    initializePagePool(totalMemorySize);
}

void PageManager::initializePagePool(int totalMemorySize)
{
    uintptr_t numPages = bytesToPageNumber(totalMemorySize);
    memoryMap_ = new BitMap(numPages);
    ASSERT(memoryMap_);

    // After the kernel_reserved_region, region for DMA.
    reservedDMAMap_ = new BitMap(bytesToPageNumber(DMA_REGION_SIZE_IN_BYTES));
    for (int i = 0; i < reservedDMAMap_->getBitsNumber(); i++) {
        int index = bytesToPageNumber(DMA_RESERVED_REGION_START + i * ARCH_PAGE_SIZE);
        memoryMap_->mark(index);
    }
}

PageEntry* PageManager::makeFirstPageDirectory()
{
    PageEntry* directory = allocatePageTable();

    // For now, 0 - 12MB is reserved
    int maxDirctoryIndex = KERNEL_RESERVED_REGION_END / ARCH_PAGE_SIZE / ARCH_PAGE_TABLE_NUM;
    for (int directoryIndex = 0; directoryIndex < maxDirctoryIndex; directoryIndex++) {
        PageEntry* table = allocatePageTable();
        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++) {
            // N.B. tell the memoryMap, this is reserved.
            memoryMap_->mark(j + directoryIndex * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(table[j]), true, true, true, ARCH_PAGE_SIZE * (directoryIndex * ARCH_PAGE_TABLE_NUM + j));
        }
        setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
    }

    PhysicalAddress vram = align4Kb(vramAddress_);

    // max vram size. 1600 * 1200 * 32bpp = 7.3MB
    int vramMaxIndex = bytesToPageNumber(vramSizeByte_);

    // Map VRAM
    for (int i = 0; i < vramMaxIndex; i++, vram += ARCH_PAGE_SIZE) {
        PageEntry* table;
        uint32_t directoryIndex = getDirectoryIndex(vram);
        uint32_t tableIndex     = getTableIndex(vram);

        if (isPresent(&(directory[directoryIndex]))) {
            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else {
            table = allocatePageTable();
            logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, vram);
    }
    return directory;
}

void PageManager::initializePageTablePool(int numTables)
{
    uint8_t* pool = (uint8_t*)malloc(numTables);
    ASSERT(pool);

    pageTablePoolAddress_ = align4Kb((PhysicalAddress)pool);

    int actualNumTables = ((int)pool + PAGE_TABLE_POOL_SIZE - pageTablePoolAddress_) / ARCH_PAGE_SIZE;
    pageTablePool_ = new BitMap(actualNumTables);
    ASSERT(pageTablePool_);
}

/*!
    \brief allocate physical page

    \param pageEntry page entry
    \param present   page present
    \param writable  page writable
    \param isUser    page access mode user
    \param address   physical address

    \return allocated physical address

    \author Higepon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* pageEntry
                                      , bool present, bool writable, bool isUser, PhysicalAddress address) const
{
    setAttribute(pageEntry, present, writable, isUser, address);
    return address;
}

/*!
    \brief allocate physical page

    \param pageEntry page entry
    \param present   page present
    \param writable  page writable
    \param isUser    page access mode user

    \return allocated physical address

    \author Higepon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* pageEntry, bool present, bool writable, bool isUser) const
{
    int foundMemory = memoryMap_->find();
    if (foundMemory == BitMap::NOT_FOUND) return -1;

    PhysicalAddress address = foundMemory * ARCH_PAGE_SIZE;
    setAttribute(pageEntry, present, writable, isUser, address);

    return address;
}

/*!
    \brief allocate physical page

    \param pageEntry page entry
    \param laddress  Linear   address
    \param paddress  Physical address
    \param present   page present
    \param writable  page writable
    \param isUser    page access mode user

    \return allocated physical address

    \author Higepon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress
                                       , bool present, bool writable, bool isUser) const
{
    PageEntry* table;
    uint32_t directoryIndex = getDirectoryIndex(laddress);
    uint32_t tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    }
    else
    {
        table = allocatePageTable();
        logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);

        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }
    return allocatePhysicalPage(&(table[tableIndex]), present, writable, isUser, paddress);
}

/*!
    \brief allocate physical page

    \param pageEntry page entry
    \param laddress  Linear   address
    \param present   page present
    \param writable  page writable
    \param isUser    page access mode user

    \return allocated physical address

    \author Higepon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress laddress
                                       , bool present, bool writable, bool isUser) const

{
    PageEntry* table;
    uint32_t directoryIndex = getDirectoryIndex(laddress);
    uint32_t tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else
    {
        table = allocatePageTable();
        logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }

    return allocatePhysicalPage(&(table[tableIndex]), present, writable, isUser);
}

// allocate physically contigous memory.
// virtio requires this.
bool PageManager::allocateContiguous(PageEntry* directory, LinearAddress laddress, int pageNum)
{
    // contigous memory address should be mapped to this Range
    // http://wiki.monaos.org/edit.php?Mona%2F%A5%E1%A5%E2%A5%EA%A5%DE%A5%C3%A5%D4%A5%F3%A5%B0
    ASSERT(laddress >= 0x90000000 && laddress <= 0x9FFFFFFF);

    // should be aligned
    ASSERT((laddress % ARCH_PAGE_SIZE) == 0);

    int foundMemory = memoryMap_->find(pageNum);
    if (foundMemory == BitMap::NOT_FOUND) return false;

    for (int i = foundMemory; i < foundMemory + pageNum; i++)
    {
        PageEntry* table;
        PhysicalAddress paddress = i * ARCH_PAGE_SIZE;
        LinearAddress address = laddress + (i - foundMemory) * ARCH_PAGE_SIZE;
        uint32_t directoryIndex = getDirectoryIndex(address);
        uint32_t tableIndex     = getTableIndex(address);

        if (isPresent(&(directory[directoryIndex])))
        {
            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else
        {
            table = allocatePageTable();
            logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, paddress);
    }
    return true;
}

void PageManager::deallocateContiguous(PageEntry* directory, LinearAddress laddress, int pageNum)
{
    returnPages(directory, laddress, pageNum * ARCH_PAGE_SIZE);
}


uint8_t* PageManager::allocateDMAMemory(PageEntry* directory, int size, bool isUser)
{
    size = (size + 4095) & 0xFFFFF000;
    int pageNum = size / ARCH_PAGE_SIZE;
    int foundMemory = reservedDMAMap_->find(pageNum);
    if (foundMemory == BitMap::NOT_FOUND) return NULL;

    /* Map DMA */
    for (int i = foundMemory; i < foundMemory + pageNum; i++)
    {
        PageEntry* table;
        PhysicalAddress address = i * ARCH_PAGE_SIZE + 0xC00000;
        uint32_t directoryIndex = getDirectoryIndex(address);
        uint32_t tableIndex     = getTableIndex(address);

        if (isPresent(&(directory[directoryIndex])))
        {
            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else
        {
            table = allocatePageTable();
            logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, address);
    }

    return (uint8_t*)(foundMemory * ARCH_PAGE_SIZE + 0xC00000);
}

void PageManager::deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size)
{
    size = (size + 4095) & 0xFFFFF000;
    int index = (address - 0x800000) / ARCH_PAGE_SIZE;

    if (index < 0 || index >= reservedDMAMap_->getBitsNumber()) return;

    for (int i = index; i < size / 4096; i++)
    {
        reservedDMAMap_->clear(i);

        uint32_t target         = i * ARCH_PAGE_SIZE + 0xC00000;
        uint32_t directoryIndex = getDirectoryIndex(target);
        uint32_t tableIndex     = getTableIndex(target);
        if (!isPresent(&directory[directoryIndex])) return;

        PageEntry* table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

        if (isPresent(&table[tableIndex]))
        {
            table[tableIndex] = 0;
        }
    }
    return;
}

/*!
    \brief create new page directory for new process

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::createNewPageDirectory() {

    PageEntry* directory = allocatePageTable();
    logprintf("%d allocated %s:%d\n", ((uint32_t)directory - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
    PageEntry* table;

    /* 0 to 12B */
    for (int i = 0; i < 12 / 4; i++)
    {
        table = allocatePageTable();
        logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
        {
            setAttribute(&(table[j]), true, true, false, i * 4 * 1024 * 1024 + 4096 * j);
        }

        setAttribute(&(directory[i]), true, true, false, (PhysicalAddress)table);
    }

    /* find 4KB align for VRAM */
    uint32_t vram = vramAddress_;
    vram = ((int)vram) & 0xFFFFF000;

    /* max vram size. 1600 * 1200 * 32bpp = 7.3MB */
    int vramMaxIndex = ((vramSizeByte_ + 4096 - 1) & 0xFFFFF000) / 4096;

    /* Map VRAM */
    for (int i = 0; i < vramMaxIndex; i++, vram += 4096) {

        uint32_t directoryIndex = getDirectoryIndex(vram);
        uint32_t tableIndex     = getTableIndex(vram);

        if (isPresent(&(directory[directoryIndex]))) {

            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, vram);
    }

    return directory;
}

void PageManager::returnPhysicalPages(PageEntry* directory)
{
    uint32_t vram = vramAddress_;
    vram = ((int)vram + 4096 - 1) & 0xFFFFF000;
    int vramIndex = getDirectoryIndex(vram);

    returnPageTable((PageEntry*)(directory[0] & 0xfffff000));
    returnPageTable((PageEntry*)(directory[1] & 0xfffff000));

    /* 0-12MB don't return */
    for (int i = 3; i < ARCH_PAGE_TABLE_NUM; i++)
    {
        // VRAM is not attached to INIT Process.
        // So we check this first.
        if (!isPresent(&(directory[i])))
        {
            continue;
        }

        // VRAM is shared, so we just returns page tables only.
        if (i == vramIndex || i == vramIndex + 1)
        {
            returnPageTable((PageEntry*)(directory[i] & 0xfffff000));
            continue;
        }

        PageEntry* table = (PageEntry*)(directory[i] & 0xfffff000);

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
        {
            if (!isPresent(&(table[j])))
            {
                continue;
            }

            PhysicalAddress address = ((uint32_t)(table[j])) & 0xfffff000;
            returnPhysicalPage(address);
        }
        returnPageTable(table);
    }
    returnPageTable(directory);
    return;
}

void PageManager::returnPages(PageEntry* directory, LinearAddress address, uint32_t size)
{
#if 0
    if (address < 0xC0000000 || (0xC0000000 + 24 * 1024 * 1024) < address) return;

    LinearAddress start = address % 4096 ? ((address + 4095) & 0xFFFFF000) : address;

        logprintf("start=%x size=%x\n", start, size);

    for (LinearAddress target = start; target + 4095 <= address + size; target += 4096)
    {
        logprintf("target=%x\n", target);
        uint32_t directoryIndex = getDirectoryIndex(target);
        uint32_t tableIndex     = getTableIndex(target);

        if (!isPresent(&directory[directoryIndex])) continue;

        PageEntry* table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

        if (isPresent(&table[tableIndex]))
        {
            PhysicalAddress paddress = ((uint32_t)(table[tableIndex])) & 0xfffff00;
            returnPhysicalPage(paddress);
            table[tableIndex] = 0;

//             /* the end of table */
//             if ((target + ARCH_PAGE_SIZE) % (4 * 1024 * 1024)) continue;

//             int counter = 0;
//             for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
//             {
//                 if (!isPresent(&table[j])) continue;
//                 counter++;
//             }

//             if (counter == 0)
//             {
//                 returnPageTable(table);
//                 directory[directoryIndex] = 0;
//             }
        }

    }


#endif

#if 0
    ASSERT(directory);

    /* out of user malloc region */
    if (address < 0xC0000000 || (0xC0000000 + 8 * 1024 * 1024) < address) return;


    uint32_t orgAddress = address;

    address = ((address + ARCH_PAGE_SIZE - 1) & 0xFFFFF000);

    for (int i = 0; (address + i * ARCH_PAGE_SIZE + ARCH_PAGE_SIZE) < (orgAddress + size); i++)
    {
        uint32_t targetAddress  = address + i * ARCH_PAGE_SIZE;
        uint32_t directoryIndex = getDirectoryIndex(targetAddress);
        uint32_t tableIndex     = getTableIndex(targetAddress);
        PageEntry* table     = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

        logprintf("targetAddress = %x \n", targetAddress);

        if (isPresent(&table[tableIndex]))
        {

            PhysicalAddress paddress = ((uint32_t)(table[tableIndex])) & 0xfffff000;
            returnPhysicalPage(paddress);
            table[tableIndex] = 0;

            /* the end of table */
            if ((targetAddress + ARCH_PAGE_SIZE) % (4 * 1024 * 1024)) continue;

            int counter = 0;
            for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
            {
                if (!isPresent(&table[j])) continue;
                counter++;
            }

            if (counter == 0)
            {
                returnPageTable(table);
                directory[directoryIndex] = 0;
            }
        }
    }
#endif
}

void PageManager::returnPageTable(PageEntry* table)
{
    PhysicalAddress address = (PhysicalAddress)table;
    logprintf("to return %d\n", (address - pageTablePoolAddress_) / ARCH_PAGE_SIZE);
    ASSERT(pageTablePool_->marked((address - pageTablePoolAddress_) / ARCH_PAGE_SIZE));
    pageTablePool_->clear((address - pageTablePoolAddress_) / ARCH_PAGE_SIZE);
}

/*!
    \brief change page directory

    \param  address physical address of page directory
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setPageDirectory(PhysicalAddress address)
{
    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

/*!
    \brief start paging

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::startPaging(PhysicalAddress address)
{
    setPageDirectory(address);
    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "eax");
}

/*!
    \brief stop paging

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::stopPaging()
{
    asm volatile("mov %%cr0       , %%eax \n"
                 "and  $0x7fffffff, %%eax \n"
                 "mov %%eax       , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief flush page cache

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::flushPageCache() const
{
    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief allocate page table

    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::allocatePageTable() const
{
    int foundMemory = pageTablePool_->find();
    if (foundMemory == BitMap::NOT_FOUND)
    {
        g_console->printf("not enough memory %s %d", __FILE__, __LINE__);
        return NULL;
    }

    uint8_t* address = (uint8_t*)(pageTablePoolAddress_ + foundMemory * ARCH_PAGE_SIZE);
    bzero(address, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    return (PageEntry*)(address);
}

#include "stdarg.h"
#include "vsprintf.h"

class NormalLogger
{
public:
    static void printf(const char*fmt, ... )
    {
#define BUFFER_SIZE 512
        char buf[BUFFER_SIZE];
        va_list ap;
        va_start(ap, fmt);

        int ret = vsprintf(buf, fmt, ap);
        va_end(ap);
        if (ret >= BUFFER_SIZE) {
            print("bufer over at StackTrace::printf");
        }
        print(buf);
    }
    static void print(const char* str)
    {
        logprintf(str);
    }
};

class StackTracer
{
public:
    StackTracer(SymbolDictionary::SymbolDictionaryMap& dictMap) : dictMap_(dictMap) {}
    template <class T> void dumpAddress(uint32_t address, SymbolDictionary::SymbolDictionary* dict)
    {
        SymbolDictionary::SymbolEntry* ent = dict->lookup(address);
        if(ent != NULL)
            T::printf("  %s: %s (%x)\n", ent->FunctionName, ent->FileName, address);
        else
            T::printf("(unknown)  %x\n", address);
    }
    template <class T> void dump(uint32_t pid, uint32_t ebp, uint32_t eip, uint32_t stackStart)
    {
        SymbolDictionary::SymbolDictionary *dict = dictMap_.get(pid);
        if(dict != NULL)
        {
            T::printf("nullpo! stack trace:\n");
            dumpAddress<T>(eip, dict);

            void**bp = (void**)ebp;
            while(bp && ((uint32_t)bp) < stackStart)
            {
                // caller = bp[1];
                dumpAddress<T>((uint32_t)bp[1], dict);
                bp = (void**)(*bp);
            }
        }
    }

private:
    SymbolDictionary::SymbolDictionaryMap& dictMap_;
};

// PageEntry* PageManager::allocatePageTable() const
// {
//     uint8_t* table;
//     table = (uint8_t*)malloc(sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM * 2);
//     checkMemoryAllocate(table, "PageManager table memory allocate");
//     table = (uint8_t*)(((int)table + 4095) & 0xFFFFF000);
//     g_console->printf("address%x", table);
//     return (PageEntry*)table;
// }

void PageManager::showCurrentStackTrace()
{
    ArchThreadInfo* i = g_currentThread->archinfo;
    StackTracer tracer(symbolDictionaryMap_);
    tracer.dump<NormalLogger>(g_currentThread->process->getPid(), i->ebp, i->eip, g_currentThread->thread->stackSegment->getStart());
}

/*!
    \brief page fault handler

    \param  address linear address of page fault point
    \param  errror  fault type
    \author Higepon
    \date   create:2003/10/15 update:2004/01/08
*/
bool PageManager::pageFaultHandler(LinearAddress address, uint32_t error, uint32_t eip)
{
    Process* current = g_currentThread->process;

    /* search shared memory segment */
    if ((error & 0x01) == ARCH_FAULT_NOT_EXIST)
    {
    List<SharedMemorySegment*>* list = current->getSharedList();
    for (int i = 0; i < list->size(); i++)
    {
        SharedMemorySegment* segment = list->get(i);
        if (segment->inRange(address))
        {
        return segment->faultHandler(address, FAULT_NOT_EXIST);
        }
    }
    }

    /* heap */
    HeapSegment* heap = current->getHeapSegment();
    if (heap->inRange(address))
    {
        return heap->faultHandler(address, FAULT_NOT_EXIST);
    }

    /* stack */
    StackSegment* stack = g_currentThread->thread->stackSegment;
    if (stack->inRange(address))
    {
        return stack->faultHandler(address, FAULT_NOT_EXIST);
    }

    if (g_isRemoteDebug) {
        gdbCatchException(VECTOR_PAGE_FAULT_EXCEPTION);
    }
#if 1
        ArchThreadInfo* i = g_currentThread->archinfo;
        logprintf("name=%s\n", g_currentThread->process->getName());
        logprintf("tid =%d\n", g_currentThread->thread->id);
        logprintf("eax=%x ebx=%x ecx=%x edx=%x\n", i->eax, i->ebx, i->ecx, i->edx);
        logprintf("esp=%x ebp=%x esi=%x edi=%x\n", i->esp, i->ebp, i->esi, i->edi);
        logprintf("cs =%x ds =%x ss =%x cr3=%x\n", i->cs , i->ds , i->ss , i->cr3);
        logprintf("eflags=%x eip=%x\n", i->eflags, i->eip);

    showCurrentStackTrace();

    // remove: if dict of this pid does not exist, just ignore.
    symbolDictionaryMap_.remove(g_currentThread->process->getPid());
#endif

        uint32_t stackButtom = current->getStackBottom(g_currentThread->thread);
        bool stackOver = address < stackButtom && stackButtom - 4096 < address;


        if (stackOver)
        {
            g_console->printf("\nstack overflow \n\n access denied.address = %x Process  killed %s thread-index=%d eip=%x\n", address, current->getName(), g_currentThread->process->getThreadIndex(g_currentThread->thread), eip);
            logprintf("\ntack overflow \n\n access denied.address = %x Process %s killed  eip=%x", address, current->getName(), eip);
        }
        else
        {
            g_console->printf("access denied.address = %x Process  killed %s thread-index=%d eip=%x\n", address, current->getName(), g_currentThread->process->getThreadIndex(g_currentThread->thread), eip);
            logprintf("access denied.address = %x Process %s killed  eip=%x", address, current->getName(), eip);
        }

        ThreadOperation::kill();
        return false;
}

/*! set page attribute

    \param  entry    page entry
    \param  present  true:page present
    \param  writable true:writable
    \param  isUser   true:user access mode
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser) const
{
    (*entry) = ((*entry) & (0xFFFFFFF8)) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);

    return true;
}

/*!
    \brief set page attribute

    \param  entry    page entry
    \param  present  true:page present
    \param  writable true:writable
    \param  isUser   true:user access mode
    \param  address  physical address
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address) const
{
    (*entry) = (address & 0xfffff000) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);

    return true;
}

/*!
    \brief set page attribute

    \param  entry    page directory
    \param  address  Linear address
    \param  present  true:page present
    \param  writable true:writable
    \param  isUser   true:user access mode
    \author Higepon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser) const
{
    PageEntry* table;
    uint32_t directoryIndex = getDirectoryIndex(address);

    //if (!isPresent(&(directory[directoryIndex]))) return false;
    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else
    {
        table = allocatePageTable();
        logprintf("%d allocated %s:%d\n", ((uint32_t)table - (uint32_t)pageTablePoolAddress_) / ARCH_PAGE_SIZE, __FILE__, __LINE__);
        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }
     return setAttribute(&(table[getTableIndex(address)]), present, writable, isUser);
}

/*!
    \brief set page absent between start and start + size(LinearAddress)

    \param  directory page directory
    \param  start     start address
    \param  size      size of absent pages

    \author Higepon
    \date   create:2003/10/27 update:
*/
void PageManager::setAbsent(PageEntry* directory, LinearAddress start, uint32_t size) const
{
    LinearAddress address;
    uint32_t directoryIndex;
    PageEntry* table;

    for (address = start; address < start + size; address += ARCH_PAGE_SIZE)
    {
        directoryIndex= getDirectoryIndex(address);
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        setAttribute(&(table[getTableIndex(address)]), false, true, true);
    }

    flushPageCache();
    return;
}

/*!
    \brief return physical page to page manager

    \param  address physical address

    \author Higepon
    \date   create:2003/10/27 update:
*/
void PageManager::returnPhysicalPage(PhysicalAddress address)
{
    memoryMap_->clear(address / ARCH_PAGE_SIZE);
}

bool PageManager::getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress)
{
    PageEntry* table;
    uint32_t directoryIndex = getDirectoryIndex(laddress);
    uint32_t tableIndex     = getTableIndex(laddress);

    /* not present */
    if (!isPresent(&(directory[directoryIndex]))) return false;

    table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

    *paddress = ((uint32_t)(table[tableIndex]) & 0xfffff800) + (laddress % 4096);

    return true;
}

void PageManager::getPagePoolInfo(uint32_t* freeNum, uint32_t* totalNum, uint32_t* pageSize)
{
    *freeNum  = memoryMap_->countClear();
    *totalNum = memoryMap_->getBitsNumber();
    *pageSize = ARCH_PAGE_SIZE;
}
