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

const int KeyBoardManager::keyMapJP109[128] = {
        0        , Keys::Escape, Keys::D1, Keys::D2, Keys::D3, Keys::D4, Keys::D5, Keys::D6,
        Keys::D7, Keys::D8, Keys::D9, Keys::D0, Keys::OemMinus, '^', Keys::Back, Keys::Tab,
        Keys::Q, Keys::W, Keys::E, Keys::R, Keys::T, Keys::Y, Keys::U, Keys::I,
        Keys::O, Keys::P, Keys::OemAt, Keys::OemOpenBrackets, Keys::Enter, Keys::LControlKey, Keys::A, Keys::S,
        Keys::D, Keys::F, Keys::G, Keys::H, Keys::J, Keys::K, Keys::L, Keys::OemSemicolon,
        Keys::OemColon, Keys::KanjiMode, Keys::LShiftKey, Keys::OemCloseBrackets, Keys::Z, Keys::X, Keys::C, Keys::V,
        Keys::B, Keys::N, Keys::M, Keys::Oemcomma, Keys::OemPeriod, Keys::Divide, Keys::RShiftKey, Keys::Multiply,
        Keys::LMenu, Keys::Space, Keys::CapsLock, Keys::F1, Keys::F2, Keys::F3, Keys::F4       , Keys::F5   ,
        Keys::F6, Keys::F7, Keys::F8, Keys::F9, Keys::F10, Keys::NumLock, Keys::Scroll, Keys::NumPad7,
        Keys::NumPad8, Keys::NumPad9, Keys::Subtract, Keys::NumPad4, Keys::NumPad5, Keys::NumPad6 , Keys::Add, Keys::NumPad1,
        Keys::NumPad2, Keys::NumPad3, Keys::NumPad0    , Keys::Decimal, 0         , 0              , 0            , Keys::F11  ,
        Keys::F12  , 0        , 0            , 0             , 0         , 0              , 0            , 0
};

const int KeyBoardManager::keyMapJP109E0[128] = {
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , KEY_PAD_ENTER, Keys::RControlKey, 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , Keys::Divide   , 0, Keys::PrintScreen,
       Keys::RMenu   , 0       , 0      , 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , 0             , 0            , 0              , 0, Keys::Home ,
       Keys::Up  , Keys::PageUp, 0      , Keys::Left, 0            , Keys::Right, 0, Keys::End    ,
       Keys::Down, Keys::PageDown, Keys::Insert, 0             , 0            , 0              , 0, 0          ,
       0             , 0       , 0      , Keys::LWin      , Keys::RWin     , KEY_MENU       , 0, 0
};

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
int KeyBoardManager::setKeyScanCode(byte scancode) {

    byte keycode   = 0; /* keycode       */
    byte modifiers = 0; /* key modifiers */

    /* first, check some scancodes */
    switch(scancode) {

      case KEYBOARD_ACK:
          return 0;
      case SPECIAL_KEY:
          isSpecialKey_ = true;
          return 0;
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
        //keycode       = keyMapE0_[scancode];
        keycode       = keyMapJP109E0[scancode];
        isSpecialKey_ = false;
    } else {
        //keycode = keyMap_[scancode];
        keycode = keyMapJP109[scancode];
    }

    switch(keycode) {

      /* invalid keycode */
      case 0:
          return 0;

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

    //printf("{%2x:%2x} ", scancode, modifiers);
    /* allocate keyinfo */
    KeyInfo* kinfo = (KeyInfo*)malloc(sizeof(KeyInfo));

    /* set virtual keyinfo */
    //toVirtual(keycode, modifiers, kinfo);
    kinfo->keycode = keycode;
    kinfo->modifiers = modifiers;
    //printf("{%2x:%2x} ", kinfo->keycode, kinfo->modifiers);
    keyInfoList_->add(kinfo);

    return 1;
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
/*    case KEY_ARROW_UP:
        info->keycode = Keys::Up;
        break;
    case KEY_ARROW_DOWN:
        info->keycode = Keys::Down;
        break;
    case KEY_ARROW_RIGHT:
        info->keycode = Keys::Right;
        break;
    case KEY_ARROW_LEFT:
        info->keycode = Keys::Left;
        break;*/
    default:
        info->keycode = keycode;
        info->modifiers = modifiers;
        break;
    }
    return;
}

