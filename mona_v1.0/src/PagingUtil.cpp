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

void PagingUtil::setup() {

    /* page_directory should be on 4Kb align */
    g_page_dir = (PTE*)malloc(sizeof(PTE) * PAGE_DIR_NUM * 2);
    if (g_page_dir == NULL) panic("Page Directory memory allocate error\n");
    for (; (dword)g_page_dir % 4096; g_page_dir++);


    return;
}
