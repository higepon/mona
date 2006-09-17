
/*!
  \file  PageManager.cpp
  \brief class PageManager

  Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X License

  \author  HigePon
  \version $Revision$
  \date   create:2003/08/23 update:$Date$
*/

#include <sys/types.h>
#include "PageManager.h"
#include "string.h"
#include "operator.h"
#include "Segments.h"
#include "global.h"

/* independent from architecture */
const byte PageManager::FAULT_NOT_EXIST;
const byte PageManager::FAULT_NOT_WRITABLE;

/* depend on architecture */
const byte PageManager::ARCH_FAULT_NOT_EXIST;
const byte PageManager::ARCH_FAULT_ACCESS_DENIED;
const byte PageManager::ARCH_FAULT_READ;
const byte PageManager::ARCH_FAULT_WRITE;
const byte PageManager::ARCH_FAULT_WHEN_KERNEL;
const byte PageManager::ARCH_FAULT_WHEN_USER;
const byte PageManager::ARCH_PAGE_PRESENT;
const byte PageManager::ARCH_PAGE_RW;
const byte PageManager::ARCH_PAGE_READ_ONLY;
const byte PageManager::ARCH_PAGE_USER;
const byte PageManager::ARCH_PAGE_KERNEL;
const int  PageManager::ARCH_PAGE_SIZE;
const int  PageManager::ARCH_PAGE_TABLE_NUM;
const int  PageManager::PAGE_TABLE_POOL_SIZE;

/*!
    \brief page management initlize

    \param totalMemorySize total system physical memory size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageManager::PageManager(dword totalMemorySize)
{
    dword pageNumber = (totalMemorySize + ARCH_PAGE_SIZE - 1) / ARCH_PAGE_SIZE;

    memoryMap_ = new BitMap(pageNumber);
    checkMemoryAllocate(memoryMap_, "PageManager memoryMap");

    /*
     * page table pool
     *
     * page table should be at 0-8MB. so use kernel memory allocation.
     */
    byte* temp = (byte*)malloc(PAGE_TABLE_POOL_SIZE);
    checkMemoryAllocate(temp, "PageManager page table pool");

    /* 4KB align */
    pageTablePoolAddress_ = (PhysicalAddress)(((int)temp + 4095) & 0xFFFFF000);

    /* page table pool bitmap */
    int poolNum = ((int)temp + PAGE_TABLE_POOL_SIZE - pageTablePoolAddress_) / ARCH_PAGE_SIZE;
    pageTablePool_ = new BitMap(poolNum);
    checkMemoryAllocate(pageTablePool_, "PageManger page table pool bitmap");
}

/*!
    \brief allocate physical page

    \param pageEntry page entry
    \param present   page present
    \param writable  page writable
    \param isUser    page access mode user
    \param address   physical address

    \return allocated physical address

    \author HigePon
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

    \author HigePon
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

    \author HigePon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress laddress, PhysicalAddress paddress
                                       , bool present, bool writable, bool isUser) const
{
    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(laddress);
    dword tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    }
    else
    {
        table = allocatePageTable();
        memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
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

    \author HigePon
    \date   create:2003/10/25 update:
*/
int PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress laddress
                                       , bool present, bool writable, bool isUser) const

{
    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(laddress);
    dword tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else
    {
        table = allocatePageTable();
        memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }

    return allocatePhysicalPage(&(table[tableIndex]), present, writable, isUser);
}

byte* PageManager::allocateDMAMemory(PageEntry* directory, int size, bool isUser)
{
    int pageNum = size / ARCH_PAGE_SIZE;
    int foundMemory = reservedDMAMap_->find(pageNum);
    if (foundMemory == BitMap::NOT_FOUND) return NULL;

    /* Map DMA */
    for (int i = foundMemory; i < foundMemory + pageNum; i++)
    {
        PageEntry* table;
        PhysicalAddress address = i * ARCH_PAGE_SIZE + 0x800000;
        dword directoryIndex = getDirectoryIndex(address);
        dword tableIndex     = getTableIndex(address);

        if (isPresent(&(directory[directoryIndex])))
        {
            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else
        {
            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, address);
    }

    return (byte*)(foundMemory * ARCH_PAGE_SIZE + 0x800000);
}

void PageManager::deallocateDMAMemory(PageEntry* directory, PhysicalAddress address, int size)
{
    int index = (address - 0x800000) / ARCH_PAGE_SIZE;

    if (index < 0 || index >= reservedDMAMap_->getBitsNumber()) return;

    for (int i = index; i < size / 4096; i++)
    {
        reservedDMAMap_->clear(i);

        dword target         = i * ARCH_PAGE_SIZE + 0x800000;
        dword directoryIndex = getDirectoryIndex(target);
        dword tableIndex     = getTableIndex(target);
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
    \brief initilize system pages

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setup(PhysicalAddress vram)
{
    PageEntry* table1 = allocatePageTable();
    PageEntry* table2 = allocatePageTable();
    g_page_directory  = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++)
    {
        memoryMap_->mark(i);
        setAttribute(&(table1[i]), true, true, true, 4096 * i);
    }

    /* allocate page to physical address 4-8MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++)
    {
        memoryMap_->mark(i + 1024);
        setAttribute(&(table2[i]), true, true, true, 4096 * 1024 + 4096 * i);
    }

    /* 8MB + 500KB is reserved for DMA */
    reservedDMAMap_ = new BitMap(125);
    for (int i = 0; i < 125; i++)
    {
        int index = (8 * 1024 * 1024 + i * ARCH_PAGE_SIZE) / ARCH_PAGE_SIZE;
        memoryMap_->mark(index);
    }

    /* Map 0-8MB */
    memset(g_page_directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    setAttribute(&(g_page_directory[0]), true, true, true, (PhysicalAddress)table1);
    setAttribute(&(g_page_directory[1]), true, true, true, (PhysicalAddress)table2);

    /* VRAM */
    vram_ = vram;

    /* find 4KB align */
    vram = ((int)vram) & 0xFFFFF000;

    /* max vram size. 1600 * 1200 * 32bpp = 7.3MB */
    int vramSizeByte = (g_vesaDetail->xResolution * g_vesaDetail->yResolution * g_vesaDetail->bitsPerPixel / 8);
    int vramMaxIndex = ((vramSizeByte + 4096 - 1) & 0xFFFFF000) / 4096;

    /* Map VRAM */
    for (int i = 0; i < vramMaxIndex; i++, vram += 4096)
    {
        PageEntry* table;
        dword directoryIndex = getDirectoryIndex(vram);
        dword tableIndex     = getTableIndex(vram);

        if (isPresent(&(g_page_directory[directoryIndex])))
        {
            table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
        } else
        {
            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(g_page_directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, vram);
    }

    setPageDirectory((PhysicalAddress)g_page_directory);

    /*
     * create kernel page directory
     * paddress == laddress
     */
    kernelDirectory_ = createKernelPageDirectory();

    startPaging();
}


PageEntry* PageManager::createKernelPageDirectory()
{
    PageEntry* directory = allocatePageTable();
    PageEntry* table;

    /* fill zero */
    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);

    /* 0 to 64MB */
    for (int i = 0; i < 64 / 4; i++)
    {
        table = allocatePageTable();

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
        {
            setAttribute(&(table[j]), true, true, false, i * 4 * 1024 * 1024 + 4096 * j);
        }

        setAttribute(&(directory[i]), true, true, false, (PhysicalAddress)table);
    }

    /* find 4KB align for VRAM */
    dword vram = vram_;
    vram = ((int)vram) & 0xFFFFF000;

    /* max vram size. 1600 * 1200 * 32bpp = 7.3MB */
    int vramSizeByte = (g_vesaDetail->xResolution * g_vesaDetail->yResolution * g_vesaDetail->bitsPerPixel / 8);
    int vramMaxIndex = ((vramSizeByte + 4096 - 1) & 0xFFFFF000) / 4096;

    /* Map VRAM */
    for (int i = 0; i < vramMaxIndex; i++, vram += 4096) {

        dword directoryIndex = getDirectoryIndex(vram);
        dword tableIndex     = getTableIndex(vram);

        if (isPresent(&(directory[directoryIndex]))) {

            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, vram);
    }

    return directory;
}

/*!
    \brief create new page directory for new process

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::createNewPageDirectory() {

    PageEntry* directory = allocatePageTable();
    PageEntry* table;

    /* fill zero */
    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);

    /* 0 to 8MB */
    for (int i = 0; i < 8 / 4; i++)
    {
        table = allocatePageTable();

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
        {
            setAttribute(&(table[j]), true, true, false, i * 4 * 1024 * 1024 + 4096 * j);
        }

        setAttribute(&(directory[i]), true, true, false, (PhysicalAddress)table);
    }

    /* find 4KB align for VRAM */
    dword vram = vram_;
    vram = ((int)vram) & 0xFFFFF000;

    /* max vram size. 1600 * 1200 * 32bpp = 7.3MB */
    int vramSizeByte = (g_vesaDetail->xResolution * g_vesaDetail->yResolution * g_vesaDetail->bitsPerPixel / 8);
    int vramMaxIndex = ((vramSizeByte + 4096 - 1) & 0xFFFFF000) / 4096;

    /* Map VRAM */
    for (int i = 0; i < vramMaxIndex; i++, vram += 4096) {

        dword directoryIndex = getDirectoryIndex(vram);
        dword tableIndex     = getTableIndex(vram);

        if (isPresent(&(directory[directoryIndex]))) {

            table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }
        setAttribute(&(table[tableIndex]), true, true, true, vram);
    }

    return directory;
}

void PageManager::returnPhysicalPages(PageEntry* directory)
{
    dword vram = vram_;
    vram = ((int)vram + 4096 - 1) & 0xFFFFF000;
    int vramIndex = getDirectoryIndex(vram);

    returnPageTable((PageEntry*)(directory[0] & 0xfffff000));
    returnPageTable((PageEntry*)(directory[1] & 0xfffff000));

    /* 0-8MB don't return */
    for (int i = 2; i < ARCH_PAGE_TABLE_NUM; i++)
    {
        /* not allocated */
        if (i == vramIndex || i == vramIndex + 1)
        {
            returnPageTable((PageEntry*)(directory[i] & 0xfffff000));
            continue;
        }

        if (!isPresent(&(directory[i])))
        {
            continue;
        }

        PageEntry* table = (PageEntry*)(directory[i] & 0xfffff000);

        for (int j = 0; j < ARCH_PAGE_TABLE_NUM; j++)
        {
            if (!isPresent(&(table[j])))
            {
                continue;
            }

            PhysicalAddress address = ((dword)(table[j])) & 0xfffff000;
            returnPhysicalPage(address);
        }
        returnPageTable(table);
    }
    returnPageTable(directory);
    return;
}

void PageManager::returnPages(PageEntry* directory, LinearAddress address, dword size)
{
#if 0
    if (address < 0xC0000000 || (0xC0000000 + 24 * 1024 * 1024) < address) return;

    LinearAddress start = address % 4096 ? ((address + 4095) & 0xFFFFF000) : address;

        logprintf("start=%x size=%x\n", start, size);

    for (LinearAddress target = start; target + 4095 <= address + size; target += 4096)
    {
        logprintf("target=%x\n", target);
        dword directoryIndex = getDirectoryIndex(target);
        dword tableIndex     = getTableIndex(target);

        if (!isPresent(&directory[directoryIndex])) continue;

        PageEntry* table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

        if (isPresent(&table[tableIndex]))
        {
            PhysicalAddress paddress = ((dword)(table[tableIndex])) & 0xfffff00;
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

    logprintf("****** [%s]address = %x size = %d\n", g_currentThread->process->getName(), address, size);

    dword orgAddress = address;

    address = ((address + ARCH_PAGE_SIZE - 1) & 0xFFFFF000);

    for (int i = 0; (address + i * ARCH_PAGE_SIZE + ARCH_PAGE_SIZE) < (orgAddress + size); i++)
    {
        dword targetAddress  = address + i * ARCH_PAGE_SIZE;
        dword directoryIndex = getDirectoryIndex(targetAddress);
        dword tableIndex     = getTableIndex(targetAddress);
        PageEntry* table     = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

        logprintf("targetAddress = %x \n", targetAddress);

        if (isPresent(&table[tableIndex]))
        {

            PhysicalAddress paddress = ((dword)(table[tableIndex])) & 0xfffff000;
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
    pageTablePool_->clear((address - pageTablePoolAddress_) / ARCH_PAGE_SIZE);
}

/*!
    \brief change page directory

    \param  address physical address of page directory
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setPageDirectory(PhysicalAddress address)
{
    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

/*!
    \brief start paging

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::startPaging()
{
    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "eax");
}

/*!
    \brief stop paging

    \author HigePon
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

    \author HigePon
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

    \author HigePon
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

    byte* address = (byte*)(pageTablePoolAddress_ + foundMemory * ARCH_PAGE_SIZE);
    return (PageEntry*)(address);
}

// PageEntry* PageManager::allocatePageTable() const
// {
//     byte* table;
//     table = (byte*)malloc(sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM * 2);
//     checkMemoryAllocate(table, "PageManager table memory allocate");
//     table = (byte*)(((int)table + 4095) & 0xFFFFF000);
//     g_console->printf("address%x", table);
//     return (PageEntry*)table;
// }

/*!
    \brief page fault handler

    \param  address linear address of page fault point
    \param  errror  fault type
    \author HigePon
    \date   create:2003/10/15 update:2004/01/08
*/
bool PageManager::pageFaultHandler(LinearAddress address, dword error, dword eip)
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

#if 1
        ArchThreadInfo* i = g_currentThread->archinfo;
        logprintf("name=%s\n", g_currentThread->process->getName());
        logprintf("eax=%x ebx=%x ecx=%x edx=%x\n", i->eax, i->ebx, i->ecx, i->edx);
        logprintf("esp=%x ebp=%x esi=%x edi=%x\n", i->esp, i->ebp, i->esi, i->edi);
        logprintf("cs =%x ds =%x ss =%x cr3=%x\n", i->cs , i->ds , i->ss , i->cr3);
        logprintf("eflags=%x eip=%x\n", i->eflags, i->eip);
#endif

        dword stackButtom = current->getStackBottom(g_currentThread->thread);
        bool stackOver = address < stackButtom && stackButtom - 4096 < address;

        g_console->printf("access denied.address = %x Process %s killed %s eip=%x\n", address, current->getName(), stackOver ? "stack overflow?" : "", eip);
        logprintf("access denied.address = %x Process %s killed %s eip=%x", address, current->getName(), stackOver ? "stack overflow?" : "", eip);

        ThreadOperation::kill();
        return true;
}

/*! set page attribute

    \param  entry    page entry
    \param  present  true:page present
    \param  writable true:writable
    \param  isUser   true:user access mode
    \author HigePon
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
    \author HigePon
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
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser) const
{
    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(address);

    //if (!isPresent(&(directory[directoryIndex]))) return false;
    if (isPresent(&(directory[directoryIndex])))
    {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else
    {
        table = allocatePageTable();
        memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }
     return setAttribute(&(table[getTableIndex(address)]), present, writable, isUser);
}

/*!
    \brief set page absent between start and start + size(LinearAddress)

    \param  directory page directory
    \param  start     start address
    \param  size      size of absent pages

    \author HigePon
    \date   create:2003/10/27 update:
*/
void PageManager::setAbsent(PageEntry* directory, LinearAddress start, dword size) const
{
    LinearAddress address;
    dword directoryIndex;
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

    \author HigePon
    \date   create:2003/10/27 update:
*/
void PageManager::returnPhysicalPage(PhysicalAddress address)
{
    memoryMap_->clear(address / ARCH_PAGE_SIZE);
}

bool PageManager::getPhysicalAddress(PageEntry* directory, LinearAddress laddress, PhysicalAddress* paddress)
{
    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(laddress);
    dword tableIndex     = getTableIndex(laddress);

    /* not present */
    if (!isPresent(&(directory[directoryIndex]))) return false;

    table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);

    *paddress = ((dword)(table[tableIndex]) & 0xfffff800) + (laddress % 4096);

    return true;
}

void PageManager::getPagePoolInfo(dword* freeNum, dword* totalNum, dword* pageSize)
{
    *freeNum  = memoryMap_->countClear();
    *totalNum = memoryMap_->getBitsNumber();
    *pageSize = ARCH_PAGE_SIZE;
}
