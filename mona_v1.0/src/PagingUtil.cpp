/*!
    \file  PagingUtil.cpp
    \brief class functions for set up Paging

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/10 update:$Date$
*/

#include<global.h>
#include<operator.h>
#include<PagingUtil.h>

#define PAGE_DIR_NUM 1024
#define PAGE_TBL_NUM 1024

void PagingUtil::setup() {

    /* page_directory should be on 4Kb align */
    g_page_dir = (PTE*)malloc(sizeof(PTE) * PAGE_DIR_NUM * 2);
    if (g_page_dir == NULL) panic("Page Directory memory allocate error\n");
    for (; (dword)g_page_dir % 4096; g_page_dir++);
    g_console->printf("%d", (dword)g_page_dir);

    g_page_tbl = (PTE*)malloc(sizeof(PTE) * PAGE_TBL_NUM * 2);
    if (g_page_tbl == NULL) panic("Page Table memory allocate error\n");
    for (; (dword)g_page_tbl % 4096; g_page_tbl++);

    makePTE(&(g_page_dir[0]), (dword)g_page_tbl, 0, 0);


    for (int i = 1; i < PAGE_DIR_NUM; i++) {

        makePTE(&(g_page_dir[i]));

    }

    for (int i = 0; i < PAGE_TBL_NUM; i++) {

        makePTE(&(g_page_tbl[i]), i* 1024, 1, 1);

    }


    setCR3((dword)g_page_dir);
    startPaging();
    return;
}

void PagingUtil::makePTE(PTE* pte) {

    pte->present          = 0;
    pte->readWrite        = 0;
    pte->user             = 0;
    pte->pageWriteThrough = 0;
    pte->pageCacheDisable = 0;
    pte->accesse          = 0;
    pte->dirty            = 0;
    pte->intelReserved    = 0;
    pte->monaAvailable    = 0;
    pte->pageBaseAddress  = 0;
    return;
}

void PagingUtil::makePTE(PTE* pte, dword physaddr, byte readWrite, byte user) {

    pte->present          = 1;
    pte->readWrite        = readWrite & 0x1;
    pte->user             = user      & 0x1;
    pte->pageWriteThrough = 0;
    pte->pageCacheDisable = 0;
    pte->accesse          = 0;
    pte->dirty            = 0;
    pte->intelReserved    = 0;
    pte->monaAvailable    = 0;
    pte->pageBaseAddress  = physaddr >> 12;
    return;
}

void PagingUtil::setCR3(dword physaddr) {

    asm volatile("movl %0   , %%eax \n"
                 "movl %%eax, %%cr3 \n" : /* no output */ : "m"(physaddr) : "eax");
}

void PagingUtil::startPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x80000000, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}

void PagingUtil::stopPaging() {

    asm volatile("mov %%cr0      , %%eax \n"
                 "or  $0x7fffffff, %%eax \n"
                 "mov %%eax      , %%cr0 \n"
                 : /* no output */
                 : /* no input  */ : "ax");
}
