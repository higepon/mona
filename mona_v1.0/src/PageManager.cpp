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
