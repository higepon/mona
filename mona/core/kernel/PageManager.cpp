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
#include "Segments.h"
#include "ihandlers.h"

PageManager::PageManager(uintptr_t systemMemorySizeByte, PhysicalAddress vramAddress, uintptr_t vramSizeByte)
  : vramAddress_(vramAddress), vramSizeByte_(vramSizeByte)
{
    initializePageTablePool(PAGE_TABLE_POOL_SIZE_BYTE);
    initializePagePool(systemMemorySizeByte);
}

void PageManager::initializePagePool(uintptr_t systemMemorySizeByte)
{
    uintptr_t numPages = bytesToPageNumber(systemMemorySizeByte);
    memoryMap_ = new BitMap(numPages);
    ASSERT(memoryMap_);

    // After the kernel_reserved_region, region for DMA.
    reservedDMAMap_ = new BitMap(bytesToPageNumber(DMA_REGION_SIZE_BYTE));
    for (int i = 0; i < reservedDMAMap_->getBitsNumber(); i++) {
        int index = bytesToPageNumber(DMA_RESERVED_REGION_START + i * ARCH_PAGE_SIZE);
        memoryMap_->mark(index);
    }
}

PageEntry* PageManager::createPageDirectory()
{
    PageEntry* directory = allocatePageTable();

    // map kernel region
    for (LinearAddress address = 0; address < KERNEL_RESERVED_REGION_END; address += ARCH_PAGE_SIZE) {
        memoryMap_->mark(address / ARCH_PAGE_SIZE); // this range is always marked as reserved.
        mapAsLinearEqPhysical(directory, address, PAGE_READ_ONLY, PAGE_KERNEL);
    }

    uint32_t vram = align4Kb(vramAddress_);
    // max vram size. 1600 * 1200 * 32bpp = 7.3MB
    int vramMaxIndex = bytesToPageNumber(vramSizeByte_);

    // map VRAM
    for (int i = 0; i < vramMaxIndex; i++, vram += ARCH_PAGE_SIZE) {
        memoryMap_->mark(vram / ARCH_PAGE_SIZE); // this range is always marked as reserved.
        mapAsLinearEqPhysical(directory, vram, PAGE_WRITABLE, PAGE_USER);
    }
    return directory;
}

void PageManager::initializePageTablePool(uintptr_t poolSizeByte)
{
    uintptr_t pool = (uintptr_t)malloc(poolSizeByte);
    ASSERT(pool);

    pageTablePoolAddress_ = align4Kb((PhysicalAddress)pool);

    uintptr_t actualNumTables = (pool + poolSizeByte - pageTablePoolAddress_) / ARCH_PAGE_SIZE;
    pageTablePool_ = new BitMap(actualNumTables);
    ASSERT(pageTablePool_);
}

int PageManager::mapOnePageByPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress, bool isWritable, bool isUser)
{
    PageEntry* table = getOrAllocateTable(directory, laddress, isWritable, isUser);
    uint32_t tableIndex = getTableIndex(laddress);
    setAttribute(&(table[tableIndex]), PAGE_PRESENT, isWritable, isUser, paddress);
    return paddress;
}

int PageManager::mapOnePage(PageEntry* directory, LinearAddress laddress, bool isWritable, bool isUser)
{
    int foundMemory = memoryMap_->find();
    if (foundMemory == BitMap::NOT_FOUND) {
        return -1;
    }
    PhysicalAddress paddress = foundMemory * ARCH_PAGE_SIZE;
    return mapOnePageByPhysicalAddress(directory, laddress, paddress, isWritable, isUser);
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
        PhysicalAddress paddress = i * ARCH_PAGE_SIZE;
        LinearAddress laddressCurrent = laddress + (i - foundMemory) * ARCH_PAGE_SIZE;
        map(directory, laddressCurrent, paddress, PAGE_WRITABLE, PAGE_USER);
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
        PhysicalAddress address = i * ARCH_PAGE_SIZE + 0xC00000;
        mapAsLinearEqPhysical(directory, address, PAGE_WRITABLE, PAGE_USER);
    }
    return (uint8_t*)(foundMemory * ARCH_PAGE_SIZE + 0xC00000);
}

void PageManager::deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size)
{
    size = (size + 4095) & 0xFFFFF000;
    int index = (address - 0x800000) / ARCH_PAGE_SIZE;

    if (index < 0 || index >= reservedDMAMap_->getBitsNumber()) return;

    for (int i = index; i < size / ARCH_PAGE_SIZE; i++)
    {
        reservedDMAMap_->clear(i);

        uint32_t target         = i * ARCH_PAGE_SIZE + 0xC00000;
        uint32_t directoryIndex = getDirectoryIndex(target);
        uint32_t tableIndex     = getTableIndex(target);
        if (!isPresent(directory[directoryIndex])) return;

        PageEntry* table = getTableAt(directory, directoryIndex);

        if (isPresent(table[tableIndex]))
        {
            table[tableIndex] = 0;
        }
    }
    return;
}

void PageManager::returnPhysicalPages(PageEntry* directory)
{
    uint32_t vram = align4Kb(vramAddress_);

    int vramIndex = getDirectoryIndex(vram);
    int vramMaxIndex = bytesToPageNumber(vramSizeByte_);
    for (int i = KERNEL_RESERVED_REGION_END / ARCH_PAGE_TABLE_NUM / ARCH_PAGE_SIZE; i < ARCH_PAGE_TABLE_NUM; i++) {
        if (!isPresent(directory[i])) {
            continue;
        }

        // VRAM is shared, so we just returns page tables only.
        if (vramIndex <= i && i < vramMaxIndex) {
            returnPageTable(getTableAt(directory, i));
            continue;
        }

        PageEntry* table = getTableAt(directory, i);

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++) {
            if (!isPresent(table[j])) {
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

    LinearAddress start = address % ARCH_PAGE_SIZE ? ((address + 4095) & 0xFFFFF000) : address;

        logprintf("start=%x size=%x\n", start, size);

    for (LinearAddress target = start; target + 4095 <= address + size; target += ARCH_PAGE_SIZE)
    {
        logprintf("target=%x\n", target);
        uint32_t directoryIndex = getDirectoryIndex(target);
        uint32_t tableIndex     = getTableIndex(target);

        if (!isPresent(&directory[directoryIndex])) continue;

        PageEntry* table = getTableAt(directory, directoryIndex)

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

void PageManager::showCurrentStackTrace()
{
    ArchThreadInfo* i = g_currentThread->archinfo;
    StackTracer tracer(symbolDictionaryMap_);
    tracer.dump<NormalLogger>(g_currentThread->process->getPid(), i->ebp, i->eip, g_currentThread->thread->stackSegment->getStart());
}

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
        bool stackOver = address < stackButtom && stackButtom - ARCH_PAGE_SIZE < address;


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

bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser)
{
    (*entry) = ((*entry) & (0xFFFFFFF8)) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);

    return true;
}

bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address)
{
    (*entry) = (address & 0xfffff000) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);

    return true;
}

bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser)
{
    PageEntry* table = getOrAllocateTable(directory, address, writable, isUser);
    return setAttribute(&(table[getTableIndex(address)]), present, writable, isUser);
}

void PageManager::setAbsent(PageEntry* directory, LinearAddress start, uint32_t size)
{
    LinearAddress address;
    uint32_t directoryIndex;
    PageEntry* table;

    for (address = start; address < start + size; address += ARCH_PAGE_SIZE) {
        directoryIndex= getDirectoryIndex(address);
        table = getTableAt(directory, directoryIndex);
        setAttribute(&(table[getTableIndex(address)]), PAGE_ABSENT, PAGE_WRITABLE, PAGE_USER);
    }

    flushPageCache();
    return;
}

void PageManager::returnPhysicalPage(PhysicalAddress address)
{
    ASSERT(memoryMap_->marked(address / ARCH_PAGE_SIZE));
    memoryMap_->clear(address / ARCH_PAGE_SIZE);
}

bool PageManager::getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress)
{
    PageEntry* table;
    uint32_t directoryIndex = getDirectoryIndex(laddress);
    uint32_t tableIndex     = getTableIndex(laddress);

    /* not present */
    if (!isPresent(directory[directoryIndex])) return false;

    table = getTableAt(directory, directoryIndex);

    *paddress = ((uint32_t)(table[tableIndex]) & 0xfffff800) + (laddress % ARCH_PAGE_SIZE);

    return true;
}

void PageManager::getPagePoolInfo(uint32_t* freeNum, uint32_t* totalNum, uint32_t* pageSize)
{
    *freeNum  = memoryMap_->countClear();
    *totalNum = memoryMap_->getBitsNumber();
    *pageSize = ARCH_PAGE_SIZE;
}
