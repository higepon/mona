/*!
    \file  PageManager.cpp
    \brief class PageManager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/08/23 update:$Date$
*/

#include <PageManager.h>
#include <BitMap.h>
#include <global.h>

PageManager::PageManager(dword totalMemorySize) {

    dword pageNumber = totalMemorySize / 4096 + totalMemorySize % 4096 ? 1 : 0;

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

    dword foundMemory = memoryMap_->find();

    /* no free memory found */
    if (foundMemory == -1) return false;

    /* set physical page */
    (*pageEntry) &= 0xFFF;
    (*pageEntry) |= (foundMemory * 4096);
    (*pageEntry) |= PAGE_PRESENT;

    return true;
}

void PageManager::setup() {

    PageEntry* table     = allocatePageTable();
    PageEntry* directory = allocatePageTable();

    /* allocate page to physical address 0-4MB */
    for (dword i = 0; i < PAGE_TABLE_NUM; i++) {

        makePageEntry(&(table[i]), 4096 * i, PAGE_PRESENT, PAGE_RW, PAGE_USER);
    }

    memset(directory, 0, sizeof(PageEntry) * PAGE_TABLE_NUM);
    makePageEntry(directory, (PhysicalAddress)table, PAGE_PRESENT, PAGE_RW, PAGE_USER);

    setPageDirectory((PhysicalAddress)directory);
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
