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
    if (memoryMap_ == NULL) panic("PageManager initilize error\n");
}

/*!
    \brief allocate physical page to page entry

    \param pageEntry page entry
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::allocatePhysicalPage(PageEntry* pageEntry) {

    int foundMemory = memoryMap_->find();

    /* no free memory found */
    if (foundMemory == -1) return false;

    /* set physical page */
    (*pageEntry) &= 0xFFF;
    (*pageEntry) |= (foundMemory * 4096);
    (*pageEntry) |= ARCH_PAGE_PRESENT;

    return true;
}

/*!
    \brief initilize system pages

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::setup() {

    PageEntry* table = allocatePageTable();
    g_page_directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        memoryMap_->mark(i);
        makePageEntry(&(table[i]), 4096 * i, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);
    }

    memset(g_page_directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(g_page_directory, (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);

    setPageDirectory((PhysicalAddress)g_page_directory);
    startPaging();
}

/*!
    \brief create new page directory for new process

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::createNewPageDirectory() {

    PageEntry* table     = allocatePageTable();
    PageEntry* directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (int i = 0; i < ARCH_PAGE_TABLE_NUM; i++) {

        makePageEntry(&(table[i]), 4096 * i, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_KERNEL);
    }

    memset(directory, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(directory, (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_KERNEL);

    dword directoryIndex = getDirectoryIndex(0xFFFFFC00);
    dword tableIndex     = getTableIndex(0xFFFFFC00);

    /* test code. stack is always 0xFFFFFFFF */
    table = allocatePageTable();
    memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
    makePageEntry(&(directory[directoryIndex]), (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, ARCH_PAGE_USER);
    allocatePhysicalPage(&(table[tableIndex]));

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
    \brief set up page entry

    \param  entry    entry to set up
    \param  address  physical address to set
    \param  present  page present bit
    \param  rw       read/write   bit
    \param  user     user/kernel  bit
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
void PageManager::makePageEntry(PageEntry* entry, PhysicalAddress address, byte present, byte rw, byte user) const {

    *entry = present | rw | user | (address & 0xfffff000);
}

/*!
    \brief allocate page table

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
PageEntry* PageManager::allocatePageTable() const {

    PageEntry* table;

    table = (PageEntry*)malloc(sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM * 2);

    if (table == NULL) panic("Page Table memory allocate error\n");
    for (; (dword)table % 4096; table++);

    return table;
}

/*!
    \brief allocate physical page

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::allocatePhysicalPage(PageEntry* directory, LinearAddress address, byte rw, byte user) {

    /* if already allocated, change attribute */

    return true;
}

/*!
    \brief page fault handler

    \param  address linear address of page fault point
    \param  errror  fault type
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::pageFaultHandler(LinearAddress address, dword error) {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(address);
    dword tableIndex     = getTableIndex(address);
    byte  user           = address >= 0x4000000 ? ARCH_PAGE_USER : ARCH_PAGE_KERNEL;

    /* physical page not exist */
    if (error & ARCH_FAULT_NOT_EXIST) {

        if (isPresent(&(g_page_directory[directoryIndex]))) {

            table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
        } else {

            table = allocatePageTable();
            memset(table, 0, sizeof(PageEntry) * ARCH_PAGE_TABLE_NUM);
            makePageEntry(&(g_page_directory[directoryIndex]), (PhysicalAddress)table, ARCH_PAGE_PRESENT, ARCH_PAGE_RW, user);
        }

        bool allocateResult = allocatePhysicalPage(&(table[tableIndex]));
        if (allocateResult) flushPageCache();

        return allocateResult;

    /* access falut */
    } else {

        g_console->printf("access denied.Process %s killed", g_current_process->name);
        g_process_manager->kill(g_current_process);
        return true;
    }
}

/*!
    \brief set page attribute

    \param  entry    page entry
    \param  present  true:page present
    \param  writable true:writable
    \param  kernel   true:kernel access mode
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* entry, bool present, bool writable, bool kernel) {

    if (!(*entry)) return false;

    (*entry) = ((*entry) & (0xFFFFFFF8)) | (present ? ARCH_PAGE_PRESENT : 0x00)
             | (writable ? ARCH_PAGE_RW : 0x00) | (kernel ? ARCH_PAGE_KERNEL : 0x00);

    return true;
}

/*!
    \brief set page attribute

    \param  entry    page directory
    \param  address  Linear address
    \param  present  true:page present
    \param  writable true:writable
    \param  kernel   true:kernel access mode
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool PageManager::setAttribute(PageEntry* directory, LinearAddress address, bool present, bool writable, bool kernel) {

    PageEntry* table;
    dword directoryIndex = getDirectoryIndex(address);

    if (!isPresent(&(directory[directoryIndex]))) return false;

     table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
     return setAttribute(&(table[getTableIndex(address)]), present, writable, kernel);
}
