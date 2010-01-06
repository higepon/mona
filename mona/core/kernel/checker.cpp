/*!
    \file   checker.cpp
    \brief  checker function

    checkers for OS

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/02 update:$Date$
*/
#include <sys/types.h>
#include "checker.h"
#include "kernel.h"
#include "GDTUtil.h"
#include "IDTUtil.h"

/*!
    \brief check size of uint8_t,uint16_t,uint32_t

    check size of uint8_t,uint16_t,uint32_t

    \author HigePon
    \date   create:2002/12/02 update:2003/06/08
*/
void checkTypeSize() {

    bool isByte     = sizeof(uint8_t)     == 1;
    bool isWord     = sizeof(uint16_t)     == 2;
    bool isDuint16_t    = sizeof(uint32_t)    == 4;
    bool isDuint64_t    = sizeof(uint64_t)    == 8;
    bool isTSS      = sizeof(TSS)      == 104;
    bool isGDTR     = sizeof(GDTR)     == 6;
    bool isSegDesc  = sizeof(SegDesc)  == 8;
    bool isGateDesc = sizeof(GateDesc) == 8;

    /* check size of some types */
    if (isByte && isWord && isDuint16_t && isTSS && isGDTR && isSegDesc && isGateDesc) {
        return;
    } else {
        panic("checkTypeSize()");
    }
    return;
}
