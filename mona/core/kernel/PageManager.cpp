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
    : systemMemorySizeByte_(systemMemorySizeByte), vramAddress_(vramAddress), vramSizeByte_(vramSizeByte)
{
    ASSERT((vramAddress_ % ARCH_PAGE_SIZE) == 0);
    initializePageTablePool(PAGE_TABLE_POOL_SIZE_BYTE);
    initializePagePool();
}

void PageManager::initializePagePool()
{
    uintptr_t numPages = bytesToPageNumber(systemMemorySizeByte_);
    memoryMap_ = new Bitmap(numPages, true);
    ASSERT(memoryMap_);

    // After the kernel_reserved_region, region for DMA.
    reservedDMAMap_ = new Bitmap(bytesToPageNumber(DMA_RESERVED_REGION_SIZE_BYTE), true);
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

    // map VRAM
    for (LinearAddress address = vramAddress_; address  < vramAddress_ + vramSizeByte_; address += ARCH_PAGE_SIZE) {
        if (address <= systemMemorySizeByte_) {
            memoryMap_->mark(address / ARCH_PAGE_SIZE); // this range is always marked as reserved.
        }
        mapAsLinearEqPhysical(directory, address, PAGE_WRITABLE, PAGE_USER);
    }
    return directory;
}

void PageManager::initializePageTablePool(uintptr_t poolSizeByte)
{
    uintptr_t pool = (uintptr_t)malloc(poolSizeByte);
    ASSERT(pool);

    pageTablePoolAddress_ = align4Kb((PhysicalAddress)pool);

    uintptr_t numTables = (pool + poolSizeByte - pageTablePoolAddress_) / ARCH_PAGE_SIZE;
    pageTablePool_ = new Bitmap(numTables, true);
    ASSERT(pageTablePool_);
}


int PageManager::mapOnePage(PageEntry* directory, PhysicalAddress& mappedAddres, LinearAddress laddress, bool isWritable, bool isUser)
{
    int foundMemory = memoryMap_->find();
    if (foundMemory == -1) {
        return M_NO_MEMORY;
    }
    PhysicalAddress paddress = foundMemory * ARCH_PAGE_SIZE;
    mapOnePageByPhysicalAddress(directory, laddress, paddress, isWritable, isUser);
    mappedAddres = paddress;
    return M_OK;
}

// allocate physically contigous memory.
// virtio requires this.
intptr_t PageManager::allocateContiguous(PageEntry* directory, LinearAddress laddress, int numPages)
{
    if (laddress < SHARED_MEMORY_REGION_START || laddress > SHARED_MEMORY_REGION_END) {
        return M_BAD_ADDRESS;
    }

    if ((laddress % ARCH_PAGE_SIZE) != 0) {
        return M_BAD_ADDRESS;
    }

    int foundMemory = memoryMap_->find(numPages);
    if (foundMemory == -1) {
        return M_NO_MEMORY;
    }

    for (int i = foundMemory; i < foundMemory + numPages; i++) {
        PhysicalAddress paddress = i * ARCH_PAGE_SIZE;
        LinearAddress laddressCurrent = laddress + (i - foundMemory) * ARCH_PAGE_SIZE;
        map(directory, laddressCurrent, paddress, PAGE_WRITABLE, PAGE_USER);
    }
    return M_OK;
}

intptr_t PageManager::deallocateContiguous(PageEntry* directory, LinearAddress laddress, int pageNum)
{
    if (laddress < SHARED_MEMORY_REGION_START || laddress > SHARED_MEMORY_REGION_END) {
        return M_BAD_ADDRESS;
    }

    if ((laddress % ARCH_PAGE_SIZE) != 0) {
        return M_BAD_ADDRESS;
    }

    for (int i = 0; i < pageNum; i++) {
        unmapOnePage(directory, laddress + i * ARCH_PAGE_SIZE);
    }
    return M_OK;
}


uint8_t* PageManager::allocateDMAMemory(PageEntry* directory, int size, bool isUser)
{
    ASSERT(false);
    size = (size + 4095) & 0xFFFFF000;
    int pageNum = size / ARCH_PAGE_SIZE;
    int foundMemory = reservedDMAMap_->find(pageNum);
    if (foundMemory == -1) return NULL;

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
    for (int i = KERNEL_RESERVED_REGION_END / ARCH_PAGE_TABLE_NUM / ARCH_PAGE_SIZE; i < ARCH_PAGE_TABLE_NUM; i++) {

        if (!isPresent(directory[i])) {
            continue;
        }
        PageEntry* table = getTableAt(directory, i);
        LinearAddress baseLinerAddress = i * ARCH_PAGE_TABLE_NUM * ARCH_PAGE_SIZE;

        // Shared memroy region will be returned by SharedMemoryObject::~SharedMemoryObject()
        if (baseLinerAddress >= 0x90000000 && baseLinerAddress < 0xA0000000) {
            returnPageTable(table);
            continue;
        }
        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++) {
            if (!isPresent(table[j])) {
                continue;
            }
            LinearAddress linearAddress = baseLinerAddress + ARCH_PAGE_SIZE * j;
            if (linearAddress >= vramAddress_ && linearAddress <= vramAddress_ + vramSizeByte_) {
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

void PageManager::returnPageTable(PageEntry* table)
{
    PhysicalAddress address = (PhysicalAddress)table;
    ASSERT(pageTablePool_->marked((address - pageTablePoolAddress_) / ARCH_PAGE_SIZE));
    pageTablePool_->clear((address - pageTablePoolAddress_) / ARCH_PAGE_SIZE);
}

void PageManager::setPageDirectory(PhysicalAddress address)
{
    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

void PageManager::startPaging(PhysicalAddress address)
{
    setPageDirectory(address);
    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "eax");
}

void PageManager::stopPaging()
{
    asm volatile("mov %%cr0       , %%eax \n"
                 "and  $0x7fffffff, %%eax \n"
                 "mov %%eax       , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::flushPageCache() const
{
    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

PageEntry* PageManager::allocatePageTable() const
{
    int foundMemory = pageTablePool_->find();
    if (foundMemory == -1)
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

bool PageManager::pageFaultHandler(ThreadInfo* threadInfo, LinearAddress address, uint32_t error)
{
    Process* process = threadInfo->process;
    Thread* thread = threadInfo->thread;

    if ((error & 0x01) == ARCH_FAULT_NOT_EXIST) {
        // SharedMemorySegment
        List<SharedMemorySegment*>* list = process->getSharedList();
        for (int i = 0; i < list->size(); i++) {
            SharedMemorySegment* segment = list->get(i);
            if (segment->inRange(address)) {
                bool ret =  segment->faultHandler(this, process, address, FAULT_NOT_EXIST);
                return ret;
            }
        }

        // Heap
        Segment* heap = process->getHeapSegment();
        if (heap->inRange(address)) {
            return heap->faultHandler(this, process, address, FAULT_NOT_EXIST);
        }

        // Stack
        Segment* stack = thread->stackSegment;
        if (stack->inRange(address)) {
            return stack->faultHandler(this, process, address, FAULT_NOT_EXIST);
        }
    }

    if (g_isRemoteDebug) {
        gdbCatchException(VECTOR_PAGE_FAULT_EXCEPTION);
    }

    showPageFault(threadInfo, address);

    ThreadOperation::kill(process, thread);
    return false;
}

void PageManager::showPageFault(ThreadInfo* threadInfo, uint32_t address)
{
    Process* process = threadInfo->process;
    Thread* thread = threadInfo->thread;
    ArchThreadInfo* i = threadInfo->archinfo;
    mona_warn("name=%s\n", process->getName());
    mona_warn("tid =%d\n", thread->id);
    mona_warn("eax=%x ebx=%x ecx=%x edx=%x\n", i->eax, i->ebx, i->ecx, i->edx);
    mona_warn("esp=%x ebp=%x esi=%x edi=%x\n", i->esp, i->ebp, i->esi, i->edi);
    mona_warn("cs =%x ds =%x ss =%x cr3=%x\n", i->cs , i->ds , i->ss , i->cr3);
    mona_warn("eflags=%x eip=%x\n", i->eflags, i->eip);

    showCurrentStackTrace();

    // remove: if dict of this pid does not exist, just ignore.
    symbolDictionaryMap_.remove(process->getPid());

    uint32_t stackButtom = process->getStackBottom(thread);
    bool stackOver = address < stackButtom && stackButtom - ARCH_PAGE_SIZE < address;

    uint32_t eip = i->eip;
    if (stackOver) {
        mona_warn("\nstack overflow \n\n access denied.address = %x Process  killed %s thread-index=%d eip=%x\n", address, process->getName(), process->getThreadIndex(thread), eip);
    } else {
        mona_warn("access denied.address = %x Process  killed %s thread-index=%d eip=%x\n", address, process->getName(), process->getThreadIndex(thread), eip);
    }
}

bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser)
{
    (*entry) = ((*entry) & (0xFFFFFFF8)) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (isUser ? ARCH_PAGE_USER : 0x00);

    return true;
}


bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser)
{
    PageEntry* table = getOrAllocateTable(directory, address, writable, isUser);
    return setAttribute(&(table[getTableIndex(address)]), present, writable, isUser);
}


void PageManager::unmapOnePage(PageEntry* directory, LinearAddress laddress, bool returnPage /* = true */)
{
    unmapPages(directory, laddress, 1, returnPage);
}

void PageManager::unmapPages(PageEntry* directory, LinearAddress start, int pageNum, bool returnPages /* = true */)
{
    LinearAddress end = start + pageNum * ARCH_PAGE_SIZE;
    for (LinearAddress laddr = start; laddr < end; laddr += ARCH_PAGE_SIZE) {
        PageEntry* table = getTable(directory, laddr);
        if (table == NULL) {
            continue;
        }

        uint32_t tableIndex = getTableIndex(laddr);
        PageEntry* entry = &table[tableIndex];
        if (returnPages) {
            PhysicalAddress paddr = *entry & 0xfffff000;
            memoryMap_->clear(paddr / ARCH_PAGE_SIZE);
        }
        setAttribute(&(table[tableIndex]), PAGE_ABSENT, PAGE_WRITABLE, PAGE_USER);
    }
    flushPageCache();
}

void PageManager::unmapRange(PageEntry* directory, LinearAddress start, LinearAddress end, bool returnPages /* = true */)
{
    unmapPages(directory, start, (end - start) / ARCH_PAGE_SIZE, returnPages);
}

void PageManager::returnPhysicalPage(PhysicalAddress address)
{
    if (!memoryMap_->marked(address / ARCH_PAGE_SIZE)) {
        logprintf("not marked %x %x\n", address, address / ARCH_PAGE_SIZE);
    }
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
