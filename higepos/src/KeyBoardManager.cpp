/*!
    \file  KeyBoardManager.cpp
    \brief class KeyBoardManager

    class KeyBoardManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/12 update:$Date$
*/
#include<KeyBoardManager.h>
#include<vga.h>

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/10/12 update:
*/
KeyBoardManager::~KeyBoardManager() {
}

/*!
    \brief private constructor

    constructor

    do nothing

    \author HigePon
    \date   create:2002/10/12 update:
*/
KeyBoardManager::KeyBoardManager() {
    /* do nothing */
    return;
}


/*!
    \brief getKeyInfo

    get key information

    \author HigePon
    \date   create:2002/10/12 update:
*/
KeyInfo* KeyBoardManager::getKeyInfo() {

    return (KeyInfo*)5;
}

/*!
    \brief setkeyInfo

    set key information

    \author HigePon
    \date   create:2002/10/12 update:
*/
void KeyBoardManager::setKeyInfo() {

    return;
}
