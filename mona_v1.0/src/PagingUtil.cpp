/*!
    \file  PagingUtil.cpp
    \brief class functions for set up Paging

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/10 update:$Date$
*/

#include <global.h>
#include <operator.h>
#include <PagingUtil.h>

#define PAGE_DIR_NUM 1024
#define PAGE_TBL_NUM 1024

#define PTE_PRESENT 0x01
#define PTE_RW      0x02
#define PTE_USER    0x04


void PagingUtil::setup() {

    /* page_directory should be on 4Kb align */
    g_page_dir = (PTE*)malloc(sizeof(PTE) * PAGE_DIR_NUM * 2);
    if (g_page_dir == NULL) panic("Page Directory memory allocate error\n");
    for (; (dword)g_page_dir % 4096; g_page_dir++);
    g_console->printf("%d", (dword)g_page_dir);

    g_page_tbl = (PTE*)malloc(sizeof(PTE) * PAGE_TBL_NUM * 2);
    if (g_page_tbl == NULL) panic("Page Table memory allocate error\n");
    for (; (dword)g_page_tbl % 4096; g_page_tbl++);

    makePTE(&(g_page_dir[0]), ((dword)g_page_tbl) >> 12, 0, 0);


    for (int i = 1; i < PAGE_DIR_NUM; i++) {

        makePTE(&(g_page_dir[i]));

    }

    for (int i = 0; i < PAGE_TBL_NUM; i++) {

        makePTE(&(g_page_tbl[i]), i, PTE_RW, PTE_USER);

    }

    for (int i = 0; i < 4 * 1024 * 1024 + 5; i++) {

	if (i != linearToPhys(i)) {
	g_console->printf("linear:%d, phys:%d\n", i, linearToPhys(i));
    }

    }

    setCR3((dword)g_page_dir);
    startPaging();
    return;
}

void PagingUtil::makePTE(PTE* pte) {

    *pte = 0;
    return;
}

void PagingUtil::makePTE(PTE* pte, dword physaddr, byte readWrite, byte user) {

    *pte = PTE_PRESENT | readWrite | user | physaddr << 12;
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
