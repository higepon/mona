/*!
    \file  KeyBoardManager.h
    \brief class KeyBoardManager

    class KeyBoardManager

    Copyright (c) 2002, 2003, 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/12 update:$Date$
*/
#ifndef _MONA_KEYBOARDMANAGER_
#define _MONA_KEYBOARDMANAGER_

#include <monapi.h>

#define MAX_KEY_BUF  255
#define SPECIAL_KEY  0xE0
#define KEYBOARD_ACK 0xFA

typedef enum {
    KEY_ESC
  , KEY_BACKSPACE
  , KEY_TAB
  , KEY_RETURN
  , KEY_LCTRL
  , KEY_LSHIFT
  , KEY_RSHIFT
  , KEY_LALT
  , KEY_CAPSLOCK
  , KEY_F1
  , KEY_F2
  , KEY_F3
  , KEY_F4
  , KEY_F5
  , KEY_F6
  , KEY_F7
  , KEY_F8
  , KEY_F9
  , KEY_F10
  , KEY_PAD_NUMLOCK
  , KEY_SCRLOCK
  , KEY_PAD_7
  , KEY_PAD_8
  , KEY_PAD_9
  , KEY_PAD_MINUS
  , KEY_PAD_4
  , KEY_PAD_5
  , KEY_PAD_6
  , KEY_PAD_PLUS
  , KEY_PAD_1
  , KEY_PAD_2
  , KEY_PAD_3
  , KEY_PAD_0
  , KEY_PAD_PERIOD
  , KEY_F11
  , KEY_F12
  , KEY_PAD_ENTERKEY_RCTRL
  , KEY_PAD_DIVIDE
  , KEY_PRTSCRN
  , KEY_RALT
  , KEY_HOME
  , KEY_ARROW_UP
  , KEY_PGUP
  , KEY_ARROW_LEFT
  , KEY_ARROW_RIGHTKEY_END
  , KEY_ARROW_DOWNKEY_PGDNKEY_INS
  , KEY_LWIN
  , KEY_RWIN
  , KEY_MENU
  , KEY_RCTRL
  , KEY_ARROW_RIGHT
  , KEY_END
  , KEY_ARROW_DOWN
  , KEY_PGDN
  , KEY_INS
  , KEY_PAD_ENTER
} keyType;


/*!
    struct for key information
*//*
typedef struct {
    unsigned int keycode;
    unsigned int modifiers;
} KeyInfo;
*/
/*!
    keyboard input management  class
*/
class KeyBoardManager {

  public:
    KeyBoardManager();
    ~KeyBoardManager();
    KeyBoardManager(const KeyBoardManager&);
    KeyBoardManager& operator = (const KeyBoardManager&);
    KeyInfo* getKeyInfo(KeyInfo* keyinfo);
    void setKeyScanCode(unsigned char);
    void init();
    void toVirtual(byte keycode, byte modifiers, KeyInfo* info);
    inline bool isInit() const {
        return isInit_;
    }

    static char toChar(int keycode);

  private:
    bool isSpecialKey_;
    bool isKeyboardId_;
    bool isShift_;
    bool isAlt_;
    bool isCtrl_;
    bool isWin_;
    bool isMenu_;
    bool isInit_;
    static const int keyMap_[128];
    static const int keyMapE0_[128];
    byte idHigh_;
    byte idLow_;
    List<KeyInfo*>* keyInfoList_;

};


#endif
