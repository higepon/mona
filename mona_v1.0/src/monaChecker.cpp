/*!
    \file   monaCherker.cpp
    \brief  checker function

    checkers for OS

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/02 update:$Date$
*/
#include <monaChecker.h>
#include <monaVga.h>
#include <monaTypes.h>
#include <monaKernel.h>

/*!
    \brief check size of byte,word,dword

    check size of byte,word,dword

    \author HigePon
    \date   create:2002/12/02 update:
*/
void checkTypeSize() {

    bool isByte  = sizeof(byte)  == 1;
    bool isWord  = sizeof(word)  == 2;
    bool isDword = sizeof(dword) == 4;

    /* check size of some types */
    if (isByte && isWord && isDword) {
        return;
    } else {
        panic("checkTypeSize()");
    }
    return;
}
