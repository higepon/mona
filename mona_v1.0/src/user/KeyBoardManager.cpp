/*!
    \file  KeyBoardManager.cpp
    \brief class KeyBoardManager

    class KeyBoardManager

    Copyright (c) 2002, 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/12 update:$Date$
*/
#include<userlib.h>
#include<KeyBoardManager.h>

const int KeyBoardManager::keyMap_[128] = {
        0        , KEY_ESC  , '1'          , '2'           , '3'       , '4'            , '5'          , '6'      ,
        '7'      , '8'      , '9'          , '0'           , '-'       , '^'            , KEY_BACKSPACE, KEY_TAB  ,
        'q'      , 'w'      , 'e'          , 'r'           , 't'       , 'y'            , 'u'          , 'i'      ,
        'o'      , 'p'      , '@'          , '['           , KEY_RETURN, KEY_LCTRL      , 'a'          , 's'      ,
        'd'      , 'f'      , 'g'          , 'h'           , 'j'       , 'k'            , 'l'          , ';'      ,
        ':'      , ']'      , KEY_LSHIFT   , ']'           , 'z'       , 'x'            , 'c'          , 'v'      ,
        'b'      , 'n'      , 'm'          , ','           , '.'       , '/'            , KEY_RSHIFT   , '*'      ,
        KEY_LALT , ' '      , KEY_CAPSLOCK , KEY_F1        , KEY_F2    , KEY_F3         , KEY_F4       , KEY_F5   ,
        KEY_F6   , KEY_F7   , KEY_F8       , KEY_F9        , KEY_F10   , KEY_PAD_NUMLOCK, KEY_SCRLOCK  , KEY_PAD_7,
        KEY_PAD_8, KEY_PAD_9, KEY_PAD_MINUS, KEY_PAD_4     , KEY_PAD_5 , KEY_PAD_6      , KEY_PAD_PLUS , KEY_PAD_1,
        KEY_PAD_2, KEY_PAD_3, KEY_PAD_0    , KEY_PAD_PERIOD, 0         , 0              , 0            , KEY_F11  ,
        KEY_F12  , 0        , 0            , 0             , 0         , 0              , 0            , 0
};

const int KeyBoardManager::keyMapE0_[128] = {
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , KEY_PAD_ENTER, KEY_RCTRL      , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , KEY_PAD_DIVIDE , 0, KEY_PRTSCRN,
       KEY_RALT      , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, KEY_HOME   ,
       KEY_ARROW_UP  , KEY_PGUP, 0      , KEY_ARROW_LEFT, 0            , KEY_ARROW_RIGHT, 0, KEY_END    ,
       KEY_ARROW_DOWN, KEY_PGDN, KEY_INS, 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , KEY_LWIN      , KEY_RWIN     , KEY_MENU       , 0, 0
};

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/10/12 update:2003/12/24
*/
KeyBoardManager::~KeyBoardManager() {
    delete keyInfoList_;
}

/*!
    \brief private constructor

    constructor

    \author HigePon
    \date   create:2002/10/12 update:2003/12/24
*/
KeyBoardManager::KeyBoardManager() {

    isInit_ = false;
    return;
}

void KeyBoardManager::init() {

    /* keyinfo list */
    keyInfoList_ = new HList<KeyInfo*>();

    isInit_ = true;
}

/*!
    \brief getKeyInfo

    get key information

    \author HigePon
    \date   create:2002/10/12 update:2003/12/24
*/
KeyInfo* KeyBoardManager::getKeyInfo(KeyInfo* keyinfo) {

    printf("[3.1%x, %s]", keyInfoList_, keyInfoList_->size());

    KeyInfo* temp = keyInfoList_->removeAt(keyInfoList_->size() - 1);

    printf("[3.2]");
    if (temp == NULL) {
        return (KeyInfo*)NULL;
    }

    printf("[3.3]");
    /* copy to keyinfo */
    memcpy(keyinfo, temp, sizeof(KeyInfo));

    printf("[3.4]");
    free(temp);
    printf("[3.5]");
    return keyinfo;
}

/*!
    \brief setKeyScanCode

    set key scancode

    \param scancode original key scan code

    \author HigePon
    \date   create:2002/10/12 update:2002/11/04
*/
void KeyBoardManager::setKeyScanCode(byte scancode) {

    byte keycode   = 0; /* keycode       */
    byte modifiers = 0; /* key modifiers */

    printf("scancode=%x ", scancode);

    printf("[2.2]");

    /* first, check some scancodes */
    switch(scancode) {

      case KEYBOARD_ACK:
          return;
      case SPECIAL_KEY:
          isSpecialKey_ = true;
          return;
    }

    printf("[2.3]");

    /* regular key */
    if (scancode & 0x80) {

        /* break code means key up */
        modifiers |= KEY_MODIFIER_UP;
        scancode  &= 0x7f;
    } else {

        /* make code means key down */
        modifiers |= KEY_MODIFIER_DOWN;
    }

    /* scancode to keycode */
    if (isSpecialKey_) {
        keycode       = keyMapE0_[scancode];
        isSpecialKey_ = false;
    } else {
        keycode = keyMap_[scancode];
    }

    printf("[2.4]");

    switch(keycode) {

      /* invalid keycode */
      case 0:
          return;

      case KEY_LSHIFT:
      case KEY_RSHIFT:
          isShift_ = !(modifiers & KEY_MODIFIER_UP);
          break;
      case KEY_LALT:
      case KEY_RALT:
          isAlt_   = !(modifiers & KEY_MODIFIER_UP);
          break;
      case KEY_LCTRL:
      case KEY_RCTRL:
          isCtrl_  = !(modifiers & KEY_MODIFIER_UP);
          break;
      case KEY_LWIN:
      case KEY_RWIN:
          isWin_   = !(modifiers & KEY_MODIFIER_UP);
          break;
      case KEY_MENU:
          isMenu_  = !(modifiers & KEY_MODIFIER_UP);
          break;
    }

    printf("[2.5]");

    if      (isShift_) modifiers |= KEY_MODIFIER_SHIFT;
    else if (isCtrl_)  modifiers |= KEY_MODIFIER_CTRL;
    else if (isAlt_)   modifiers |= KEY_MODIFIER_ALT;
    else if (isWin_)   modifiers |= KEY_MODIFIER_WIN;
    else if (isMenu_)  modifiers |= KEY_MODIFIER_MENU;

    printf("[2.6]");

    /* allocate keyinfo */
    KeyInfo* kinfo = (KeyInfo*)malloc(sizeof(KeyInfo));

    printf("[2.7]");

    /* set keyinfo */
    kinfo->keycode   = keycode;
    printf("[2.71]");
    kinfo->modifiers = modifiers;
    printf("[2.72]");
    keyInfoList_->add(kinfo);

    printf("[2.8]");
    return;
}
