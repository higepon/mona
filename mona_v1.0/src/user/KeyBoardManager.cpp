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

    KeyInfo* temp = keyInfoList_->removeAt(keyInfoList_->size() - 1);

    if (temp == NULL) {
        return (KeyInfo*)NULL;
    }

    /* copy to keyinfo */
    memcpy(keyinfo, temp, sizeof(KeyInfo));

    free(temp);
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

    /* first, check some scancodes */
    switch(scancode) {

      case KEYBOARD_ACK:
          return;
      case SPECIAL_KEY:
          isSpecialKey_ = true;
          return;
    }

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

    if      (isShift_) modifiers |= KEY_MODIFIER_SHIFT;
    else if (isCtrl_)  modifiers |= KEY_MODIFIER_CTRL;
    else if (isAlt_)   modifiers |= KEY_MODIFIER_ALT;
    else if (isWin_)   modifiers |= KEY_MODIFIER_WIN;
    else if (isMenu_)  modifiers |= KEY_MODIFIER_MENU;

    /* allocate keyinfo */
    KeyInfo* kinfo = (KeyInfo*)malloc(sizeof(KeyInfo));

    /* set virtual keyinfo */
    toVirtual(keycode, modifiers, kinfo);
    keyInfoList_->add(kinfo);

    return;
}

void KeyBoardManager::toVirtual(byte keycode, byte modifiers, KeyInfo* info) {

    info->modifiers = modifiers;

    //    printf("%x", keycode);

    switch(keycode) {

    case('a'):
        info->keycode = VK_A;
        break;
    case('b'):
        info->keycode = VK_B;
        break;
    case('c'):
        info->keycode = VK_C;
        break;
    case('d'):
        info->keycode = VK_D;
        break;
    case('e'):
        info->keycode = VK_E;
        break;
    case('f'):
        info->keycode = VK_F;
        break;
    case('g'):
        info->keycode = VK_G;
        break;
    case('h'):
        info->keycode = VK_H;
        break;
    case('i'):
        info->keycode = VK_I;
        break;
    case('j'):
        info->keycode = VK_J;
        break;
    case('k'):
        info->keycode = VK_K;
        break;
    case('l'):
        info->keycode = VK_L;
        break;
    case('m'):
        info->keycode = VK_M;
        break;
    case('n'):
        info->keycode = VK_N;
        break;
    case('o'):
        info->keycode = VK_O;
        break;
    case('p'):
        info->keycode = VK_P;
        break;
    case('q'):
        info->keycode = VK_Q;
        break;
    case('r'):
        info->keycode = VK_R;
        break;
    case('s'):
        info->keycode = VK_S;
        break;
    case('t'):
        info->keycode = VK_T;
        break;
    case('u'):
        info->keycode = VK_U;
        break;
    case('v'):
        info->keycode = VK_V;
        break;
    case('w'):
        info->keycode = VK_W;
        break;
    case('x'):
        info->keycode = VK_X;
        break;
    case('y'):
        info->keycode = VK_Y;
        break;
    case('z'):
        info->keycode = VK_Z;
        break;
    case(0x2E):
        info->keycode = VK_PERIOD;
        break;
    case(0x03):
        info->keycode = VK_ENTER;
        break;
    case(0x01):
        info->keycode = VK_BACKSPACE;
        break;
    default:
        info->keycode = keycode;
        info->modifiers = modifiers;
        break;
    }
    return;
}

char KeyBoardManager::toChar(int keycode) {

    char result;

    switch(keycode) {

    case(VK_A):
        result = 'A';
        break;
    case(VK_B):
        result = 'B';
        break;
    case(VK_C):
        result = 'C';
        break;
    case(VK_D):
        result = 'D';
        break;
    case(VK_E):
        result = 'E';
        break;
    case(VK_F):
        result = 'F';
        break;
    case(VK_G):
        result = 'G';
        break;
    case(VK_H):
        result = 'H';
        break;
    case(VK_I):
        result = 'I';
        break;
    case(VK_J):
        result = 'J';
        break;
    case(VK_K):
        result = 'K';
        break;
    case(VK_L):
        result = 'L';
        break;
    case(VK_M):
        result = 'M';
        break;
    case(VK_N):
        result = 'N';
        break;
    case(VK_O):
        result = 'O';
        break;
    case(VK_P):
        result = 'P';
        break;
    case(VK_Q):
        result = 'Q';
        break;
    case(VK_R):
        result = 'R';
        break;
    case(VK_S):
        result = 'S';
        break;
    case(VK_T):
        result = 'T';
        break;
    case(VK_U):
        result = 'U';
        break;
    case(VK_V):
        result = 'V';
        break;
    case(VK_W):
        result = 'W';
        break;
    case(VK_X):
        result = 'X';
        break;
    case(VK_Y):
        result = 'Y';
        break;
    case(VK_Z):
        result = 'Z';
        break;
    case(VK_PERIOD):
        result = '.';
        break;
    default:
        result = ' ';
        break;
    }
    return result;
}
