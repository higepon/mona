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
#include <monapi.h>
#include <KeyboardManager.h>
#include <monapi/Keys.h>

using namespace MonAPI;

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

    switch(keycode) {

    case('a'):
        info->keycode = Keys::A;
        break;
    case('b'):
        info->keycode = Keys::B;
        break;
    case('c'):
        info->keycode = Keys::C;
        break;
    case('d'):
        info->keycode = Keys::D;
        break;
    case('e'):
        info->keycode = Keys::E;
        break;
    case('f'):
        info->keycode = Keys::F;
        break;
    case('g'):
        info->keycode = Keys::G;
        break;
    case('h'):
        info->keycode = Keys::H;
        break;
    case('i'):
        info->keycode = Keys::I;
        break;
    case('j'):
        info->keycode = Keys::J;
        break;
    case('k'):
        info->keycode = Keys::K;
        break;
    case('l'):
        info->keycode = Keys::L;
        break;
    case('m'):
        info->keycode = Keys::M;
        break;
    case('n'):
        info->keycode = Keys::N;
        break;
    case('o'):
        info->keycode = Keys::O;
        break;
    case('p'):
        info->keycode = Keys::P;
        break;
    case('q'):
        info->keycode = Keys::Q;
        break;
    case('r'):
        info->keycode = Keys::R;
        break;
    case('s'):
        info->keycode = Keys::S;
        break;
    case('t'):
        info->keycode = Keys::T;
        break;
    case('u'):
        info->keycode = Keys::U;
        break;
    case('v'):
        info->keycode = Keys::V;
        break;
    case('w'):
        info->keycode = Keys::W;
        break;
    case('x'):
        info->keycode = Keys::X;
        break;
    case('y'):
        info->keycode = Keys::Y;
        break;
    case('z'):
        info->keycode = Keys::Z;
        break;
    case('1'):
        info->keycode = Keys::D1;
        break;
    case('2'):
        info->keycode = Keys::D2;
        break;
    case('3'):
        info->keycode = Keys::D3;
        break;
    case('4'):
        info->keycode = Keys::D4;
        break;
    case('5'):
        info->keycode = Keys::D5;
        break;
    case('6'):
        info->keycode = Keys::D6;
        break;
    case('7'):
        info->keycode = Keys::D7;
        break;
    case('8'):
        info->keycode = Keys::D8;
        break;
    case('9'):
        info->keycode = Keys::D9;
        break;
    case('0'):
        info->keycode = Keys::D0;
        break;
    case(0x15):
        info->keycode = Keys::NumPad7;
        break;
    case(0x16):
        info->keycode = Keys::NumPad8;
        break;
    case(0x17):
        info->keycode = Keys::NumPad9;
        break;
    case(0x18):
        info->keycode = Keys::Subtract;
        break;
    case(0x19):
        info->keycode = Keys::NumPad4;
        break;
    case(0x1A):
        info->keycode = Keys::NumPad5;
        break;
    case(0x1B):
        info->keycode = Keys::NumPad6;
        break;
    case(0x1C):
        info->keycode = Keys::Add;
        break;
    case(0x1D):
        info->keycode = Keys::NumPad1;
        break;
    case(0x1E):
        info->keycode = Keys::NumPad2;
        break;
    case(0x1F):
        info->keycode = Keys::NumPad3;
        break;
    case(0x20):
        info->keycode = Keys::Space;
        break;
    case(0x21):
        info->keycode = Keys::Decimal;
        break;
    case(0x2E):
        info->keycode = Keys::Decimal;
        break;
    case(0x2F):
        info->keycode = Keys::Divide;
        break;
    case(0x03):
        info->keycode = Keys::Enter;
        break;
    case(0x01):
        info->keycode = Keys::Back;
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

    case(Keys::A):
        result = 'A';
        break;
    case(Keys::B):
        result = 'B';
        break;
    case(Keys::C):
        result = 'C';
        break;
    case(Keys::D):
        result = 'D';
        break;
    case(Keys::E):
        result = 'E';
        break;
    case(Keys::F):
        result = 'F';
        break;
    case(Keys::G):
        result = 'G';
        break;
    case(Keys::H):
        result = 'H';
        break;
    case(Keys::I):
        result = 'I';
        break;
    case(Keys::J):
        result = 'J';
        break;
    case(Keys::K):
        result = 'K';
        break;
    case(Keys::L):
        result = 'L';
        break;
    case(Keys::M):
        result = 'M';
        break;
    case(Keys::N):
        result = 'N';
        break;
    case(Keys::O):
        result = 'O';
        break;
    case(Keys::P):
        result = 'P';
        break;
    case(Keys::Q):
        result = 'Q';
        break;
    case(Keys::R):
        result = 'R';
        break;
    case(Keys::S):
        result = 'S';
        break;
    case(Keys::T):
        result = 'T';
        break;
    case(Keys::U):
        result = 'U';
        break;
    case(Keys::V):
        result = 'V';
        break;
    case(Keys::W):
        result = 'W';
        break;
    case(Keys::X):
        result = 'X';
        break;
    case(Keys::Y):
        result = 'Y';
        break;
    case(Keys::Z):
        result = 'Z';
        break;
    case(Keys::D0):
        result = '0';
        break;
    case(Keys::D1):
        result = '1';
        break;
    case(Keys::D2):
        result = '2';
        break;
    case(Keys::D3):
        result = '3';
        break;
    case(Keys::D4):
        result = '4';
        break;
    case(Keys::D5):
        result = '5';
        break;
    case(Keys::D6):
        result = '6';
        break;
    case(Keys::D7):
        result = '7';
        break;
    case(Keys::D8):
        result = '8';
        break;
    case(Keys::D9):
        result = '9';
        break;
    case(Keys::NumPad0):
        result = '0';
        break;
    case(Keys::NumPad1):
        result = '1';
        break;
    case(Keys::NumPad2):
        result = '2';
        break;
    case(Keys::NumPad3):
        result = '3';
        break;
    case(Keys::NumPad4):
        result = '4';
        break;
    case(Keys::NumPad5):
        result = '5';
        break;
    case(Keys::NumPad6):
        result = '6';
        break;
    case(Keys::NumPad7):
        result = '7';
        break;
    case(Keys::NumPad8):
        result = '8';
        break;
    case(Keys::NumPad9):
        result = '9';
        break;
    case(Keys::Decimal):
        result = '.';
        break;
    case(Keys::Add):
        result = '+';
        break;
    case(Keys::Subtract):
        result = '-';
        break;
    case(Keys::Multiply):
        result = '*';
        break;
    case(Keys::Divide):
        result = '/';
        break;
    case(Keys::Space):
        result = ' ';
        break;
    default:
        result = ' ';
        break;
    }
    return result;
}
