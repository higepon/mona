/*!
    \file   monaChecker.cpp
    \brief  checker function

    checkers for OS

    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/02 update:$Date$
*/
#include <monaChecker.h>
#include <monaTypes.h>
#include <monaKernel.h>
#include <IA32MemoryManager.h>
#include <kthread.h>

/*!
    \brief check size of byte,word,dword

    check size of byte,word,dword

    \author HigePon
    \date   create:2002/12/02 update:2003/03/01
*/
void checkTypeSize() {

    bool isByte    = sizeof(byte)    == 1;
    bool isWord    = sizeof(word)    == 2;
    bool isDword   = sizeof(dword)   == 4;
    bool isTSS     = sizeof(TSS)     == 104;
    bool isPTE     = sizeof(PTE)     == 4;
    bool isGDTR    = sizeof(GDTR)    == 6;
    bool isGDT     = sizeof(GDT)     == 8;
    bool isKthread = sizeof(Kthread) == 56;

    /* check size of some types */
    if (isByte && isWord && isDword && isTSS && isPTE && isGDTR && isGDT && isKthread) {
        return;
    } else {
        panic("checkTypeSize()");
    }
    return;
}
