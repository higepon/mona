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
    \brief setKeyScanCode

    set key scancode

    \param scancode original key scan code

    \author HigePon
    \date   create:2002/10/12 update:2002/10/19
*/
void KeyBoardManager::setKeyScanCode(unsigned char scancode) {


    /* check spceial key */

        /* if spceial key flg = true */

    /* if not spceial key and flg = true */
        /* do something */


    /* regular key */

    return;
}
