/*!
    \file  GDTUtil.cpp
    \brief class functions for set up GDT

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/06 update:$Date$
*/

#include<global.h>
#include<GDTUtil.h>

/*!
    \brief set segment descriptor

    \param  desc  segment descriptor
    \param  base  base address 32bit
    \param  limit limit        20bit
    \param  type  segment type 12bit
    \author HigePon
    \date   create:2003/06/06 update:
*/
void GDTUtil::setSegDesc(SegDesc* desc, dword base, dword limit, word type) {

    desc->baseL  = (word)(base & 0xFFFF);
    desc->baseM  = (byte)((base >> 16) & 0xFF);
    desc->baseH  = (byte)((base >> 24) & 0xFF);
    desc->limitL = (word)(limit & 0xFFFF);
    desc->limitH = (byte) (((limit >> 16) & 0x0F) | 0xC0); /* 4KB unit & 32bit segment */
    desc->type   = type;
    return;
}

