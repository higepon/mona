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
#include <BitMap.h>
#include <types.h>
#include <string.h>
#include <operator.h>
#include <global.h>

PageManager::PageManager(dword totalMemorySize) {

    dword pageNumber = totalMemorySize / 4096 + ((totalMemorySize % 4096) ? 1 : 0);

    memoryMap_ = new BitMap(pageNumber);
    if (memoryMap_ == NULL) panic("PageManager initilize error\n");

}

void PageManager::flushPageCache() const {

    asm volatile("mov %%cr3, %%eax\n"
                 "mov %%eax, %%cr3\n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

bool PageManager::allocatePhysicalPage(PageEntry* pageEntry) {

    int foundMemory = memoryMap_->find();

    /* no free memory found */
    if (foundMemory == -1) return false;

    /* set physical page */
    (*pageEntry) &= 0xFFF;
    (*pageEntry) |= (foundMemory * 4096);
    (*pageEntry) |= PAGE_PRESENT;

    //    makePageEntry(pageEntry, foundMemory*4096, PAGE_PRESENT, PAGE_RW, PAGE_USER);

    info(DEBUG, "entry=%x", (dword)(*pageEntry));

    return true;
}

void PageManager::setup() {

    PageEntry* table = allocatePageTable();
    g_page_directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (dword i = 0; i < PAGE_TABLE_NUM; i++) {

        memoryMap_->mark(i);
        makePageEntry(&(table[i]), 4096 * i, PAGE_PRESENT, PAGE_RW, PAGE_USER);
    }

    memset(g_page_directory, 0, sizeof(PageEntry) * PAGE_TABLE_NUM);
    makePageEntry(g_page_directory, (PhysicalAddress)table, PAGE_PRESENT, PAGE_RW, PAGE_USER);

    setPageDirectory((PhysicalAddress)g_page_directory);
    startPaging();


}

void PageManager::setPageDirectory(PhysicalAddress address) {

    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(address) : "eax");
}

void PageManager::startPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::stopPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x7fffffff, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PageManager::makePageEntry(PageEntry* entry, PhysicalAddress address, byte present, byte rw, byte user) const {

    *entry = present | rw | user | (address & 0xfffff000);
}

PageEntry* PageManager::allocatePageTable() const {

    PageEntry* table;

    table = (PageEntry*)malloc(sizeof(PageEntry) * PAGE_TABLE_NUM * 2);

    if (table == NULL) panic("Page Table memory allocate error\n");
    for (; (dword)table % 4096; table++);

    return table;
}

bool PageManager::pageFaultHandler(LinearAddress address) {

    PageEntry* table;
    dword directoryIndex = address >> 22;
    dword tableIndex     = (address >> 12) & 0x3FF;
    byte user            = address >= 0x4000000 ? PAGE_USER : PAGE_KERNEL;

    if (isPresent(&(g_page_directory[directoryIndex]))) {

        table = (PageEntry*)(g_page_directory[directoryIndex] & 0xfffff000);
    } else {

        table = allocatePageTable();
        memset(table, 0, sizeof(PageEntry) * PAGE_TABLE_NUM);
        makePageEntry(&(g_page_directory[directoryIndex]), (PhysicalAddress)table, PAGE_PRESENT, PAGE_RW, user);
    }

    return allocatePhysicalPage(&(table[tableIndex]));
}
