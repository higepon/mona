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
#include<higeVga.h>
#include<higeIo.h>


const int KeyBoardManager::keyMap_[128] = {
        0        , KEY_ESC  , '1'          , '2'           , '3'       , '4'            , '5'          , '6'      ,
        '7'      , '8'      , '9'          , '0'           , '-'       , '='            , KEY_BACKSPACE, KEY_TAB  ,
        'q'      , 'w'      , 'e'          , 'r'           , 't'       , 'y'            , 'u'          , 'i'      ,
        'o'      , 'p'      , '@'          , '['           , KEY_RETURN, KEY_LCTRL      , 'a'          , 's'      ,
        'd'      , 'f'      , 'g'          , 'h'           , 'j'       , 'k'            , 'l'          , ';'      ,
        ':'      , ']'      , KEY_LSHIFT   , '\\'          , 'z'       , 'x'            , 'c'          , 'v'      ,
        'b'      , 'n'      , 'm'          , ','           , '.'       , '/'            , KEY_RSHIFT   , '*'      ,
        KEY_LALT , ' '      , KEY_CAPSLOCK , KEY_F1        , KEY_F2    , KEY_F3         , KEY_F4       , KEY_F5   ,
        KEY_F6   , KEY_F7   , KEY_F8       , KEY_F9        , KEY_F10   , KEY_PAD_NUMLOCK, KEY_SCRLOCK  , KEY_PAD_7,
        KEY_PAD_8, KEY_PAD_9, KEY_PAD_MINUS, KEY_PAD_4     , KEY_PAD_5 , KEY_PAD_6      , KEY_PAD_PLUS , KEY_PAD_1,
        KEY_PAD_2, KEY_PAD_3, KEY_PAD_0    , KEY_PAD_PERIOD, 0         , 0              , 0            , KEY_F11  ,
        KEY_F12  , 0        , 0            , 0             , 0         , 0              , 0            , 0
    };
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

    \author HigePon
    \date   create:2002/10/12 update:
*/
KeyBoardManager::KeyBoardManager() {

    keyBufIndex_       = 0; /* index is 0 */
    keyBufGottenIndex_ = 0; /* index is 0 */
    return;
}


/*!
    \brief getKeyInfo

    get key information

    \author HigePon
    \date   create:2002/10/12 update:
*/
KeyInfo* KeyBoardManager::getKeyInfo() {

    KeyInfo* result = &(keyInfo_[keyBufGottenIndex_]);
    keyBufGottenIndex_++;

    /* check max */
    if (keyBufGottenIndex_ >= MAX_KEY_BUF) {
        keyBufGottenIndex_ = 0;
    }

    return result;
}

/*!
    \brief setKeyScanCode

    set key scancode

    \param scancode original key scan code

    \author HigePon
    \date   create:2002/10/12 update:2002/10/26
*/
void KeyBoardManager::setKeyScanCode(unsigned char scancode) {

    H_BYTE modifiers = 0; /* key modifiers */

    _sys_printf("scancode=%x ", scancode);

    /* first, check some scancodes */
    switch(scancode) {

      case KEYBOARD_ACK:
          return;
      case SPECIAL_KEY:
          isSpecialKey_ = true;
          return;
    }

        /* if spceial key flg = true */

    /* if not spceial key and flg = true */
        /* do something */


    /* regular key */
    if (scancode & 0x80) {

        /* break code means key up */
        modifiers |= KEY_MODIFIER_UP;
        scancode  &= 0x7f;
    } else {

        /* make code means key down */
        modifiers |= KEY_MODIFIER_DOWN;
    }


    keyBufIndex_++;
    if (keyBufIndex_ >= MAX_KEY_BUF) {
        keyBufIndex_ = 0;
    }


    keyInfo_[keyBufIndex_].keycode   = keyMap_[scancode];
    keyInfo_[keyBufIndex_].modifiers = modifiers;
    _sys_printf("%s char=\"%c\" \n", (modifiers & KEY_MODIFIER_DOWN)? "down":"up  "
                                   , (char)keyMap_[scancode]);
    return;
}

/*!
    \brief print info about keyboard

    print keyboard infomation ID

    \author HigePon
    \date   create:2002/10/25 update:2002/10/25
*/
void KeyBoardManager::printInfo() const {

    return;
}
