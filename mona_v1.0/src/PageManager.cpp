/*!
  \file  PageManager.cpp
  \brief class PageManager

  Copyright (c) 2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/08/23 update:$Date$
*/

#include <PageManager.h>
#include <types.h>
#include <string.h>
#include <operator.h>
#include <Segments.h>
#include <global.h>

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

/*!
    \brief page management initlize

    \param totalMemorySize total system physical memory size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageManager::PageManager(dword totalMemorySize) {

    dword pageNumber = totalMemorySize / ARCH_PAGE_SIZE + ((totalMemorySize % ARCH_PAGE_SIZE) ? 1 : 0);

    memoryMap_ = new BitMap(pageNumber);
    checkMemoryAllocate(memoryMap_, "PageManager memoryMap");
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
                                      , bool present, bool writable, bool isUser, PhysicalAddress address) const {

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
int PageManager::allocatePhysicalPage(PageEntry* pageEntry, bool present, bool writable, bool isUser) const {

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
                                       , bool present, bool writable, bool isUser) const {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(laddress);
    dword tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex]))) {
        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else {
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
                                       , bool present, bool writable, bool isUser) const {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(laddress);
    dword tableIndex     = getTableIndex(laddress);

    if (isPresent(&(directory[directoryIndex]))) {

        table = (PageEntry*)(directory[directoryIndex] & 0xfffff000);
    } else {

        table = allocatePageTable();
        memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
        setAttribute(&(directory[directoryIndex]), true, writable, isUser, (PhysicalAddress)table);
    }

    return allocatePhysicalPage(&(table[tableIndex]), present, writable, isUser);
}

/*!
    \brief initilize system pages

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setup() {

    PageEntry* table1 = allocatePageTable();
    PageEntry* table2 = allocatePageTable();
    g_page_directory  = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        memoryMap_->mark(i);
        setAttribute(&(table1[i]), true, true, true, 4096 * i);
    }

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        memoryMap_->mark(i + 4096);
        setAttribute(&(table2[i]), true, true, true, 4096 * 4096 + 4096 * i);
    }

    memset(g_page_directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    setAttribute(&(g_page_directory[0]), true, true, true, (PhysicalAddress)table1);
    setAttribute(&(g_page_directory[1]), true, true, true, (PhysicalAddress)table2);

    setPageDirectory((PhysicalAddress)g_page_directory);
    startPaging();
}

/*!
    \brief create new page directory for new process

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::createNewPageDirectory() {

    PageEntry* table1    = allocatePageTable();
    PageEntry* table2    = allocatePageTable();
    PageEntry* directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        setAttribute(&(table1[i]), true, true, false, 4096 * i);
    }

    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        setAttribute(&(table2[i]), true, true, false, 4096 * 4096 + 4096 * i);
    }

    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    setAttribute(&(directory[0]), true, true, false, (PhysicalAddress)table1);
    setAttribute(&(directory[1]), true, true, false, (PhysicalAddress)table2);
    return directory;
}

PageEntry* PageManager::createNewPageDirectoryForV86() {

    PageEntry* table1    = allocatePageTable();
    PageEntry* table2    = allocatePageTable();
    PageEntry* directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        /* user access ok beyond 1MB */
        if (i < 256) {
            setAttribute(&(table1[i]), true, true, true, 4096 * i);
        } else {
            setAttribute(&(table1[i]), true, true, false, 4096 * i);
        }
    }

    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        setAttribute(&(table2[i]), true, true, false, 4096 * 4096 + 4096 * i);
    }

    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    setAttribute(&(directory[0]), true, true, true, (PhysicalAddress)table1);
    setAttribute(&(directory[1]), true, true, false, (PhysicalAddress)table2);
    return directory;
}

/*!
    \brief change page directory

    \param  address physical address of page directory
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setPageDirectory(PhysicalAddress address) {

    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

/*!
    \brief start paging

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::startPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief stop paging

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::stopPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x7fffffff, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

/*!
    \brief flush page cache

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::flushPageCache() const {

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
PageEntry* PageManager::allocatePageTable() const {

    byte* table;
    table = (byte*)malloc(sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM * 2);
    checkMemoryAllocate(table, "PageManager table memory allocate");
    for (; (int)table % 4096; table++);
    return (PageEntry*)table;
}

/*!
    \brief page fault handler

    \param  address linear address of page fault point
    \param  errror  fault type
    \author HigePon
    \date   create:2003/10/15 update:2004/01/08
*/
bool PageManager::pageFaultHandler(LinearAddress address, dword error) {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(address);
    dword tableIndex     = getTableIndex(address);
    Process* current     = g_processManager->getCurrentProcess();

    dword realcr3;
    asm volatile("mov %%cr3, %%eax  \n"
                 "mov %%eax, %0     \n"
                 : "=m"(realcr3):: "eax");

    if (realcr3 != (dword)current->getPageDirectory()) {
        g_console->printf("PageFault[%s] addr=%x, error=%x\n", current->getName(), address, error);
        g_console->printf("realCR3=%x processCR3=%x\n", realcr3, current->getPageDirectory());
    }

    /* search shared memory segment */
    List<SharedMemorySegment*>* list = current->getSharedList();
    for (int i = 0; i < list->size(); i++) {
        SharedMemorySegment* segment = list->get(i);

        if (segment->inRange(address)) {
            return segment->faultHandler(address, FAULT_NOT_EXIST);
        }
    }

    /* heap */
    HeapSegment* heap = current->getHeapSegment();
    if (heap->inRange(address)) {
        return heap->faultHandler(address, FAULT_NOT_EXIST);
    }

    /* physical page not exist */
    if (error & ARCH_FAULT_NOT_EXIST) {

        if (isPresent(&(g_page_directory[directoryIndex]))) {

            table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            setAttribute(&(g_page_directory[directoryIndex]), true, true, true, (PhysicalAddress)table);
        }

        bool allocateResult = allocatePhysicalPage(&(table[tableIndex]), true, true, true);
        if (allocateResult) flushPageCache();

        return allocateResult;

    /* access falut */
    } else {

        g_console->printf("access denied.address = %x Process %s killed", address, current->getName());
        g_processManager->kill(current);
        schedule();
        return true;
    }
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
bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser) const {


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
bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool isUser, PhysicalAddress address) const {

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
bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool isUser) const {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(address);

    //if (!isPresent(&(directory[directoryIndex]))) return false;

     table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
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
void PageManager::setAbsent(PageEntry* directory, LinearAddress start, dword size) const {

    LinearAddress address;
    dword directoryIndex;
    PageEntry* table;

    for (address = start; address < start + size; address += ARCH_PAGE_SIZE) {

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
void PageManager::returnPhysicalPage(PhysicalAddress address) {

    memoryMap_->clear(address / ARCH_PAGE_SIZE);
}
