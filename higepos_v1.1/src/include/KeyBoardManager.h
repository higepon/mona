/*!
    \file  KeyBoardManager.h
    \brief class KeyBoardManager

    class KeyBoardManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/10/12 update:$Date$
*/
#ifndef _HIGEPOS_KEYBOARDMANAGER_
#define _HIGEPOS_KEYBOARDMANAGER_

#include<higeTypes.h>

#define MAX_KEY_BUF 255
#define SPECIAL_KEY 0xE0

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
} keyType;


/*!
    struct for key information
*/
typedef struct {
    unsigned int keycode;
    unsigned int addcode;
} KeyInfo;

/*!
    keyboard input management  class
    single pattern  applyes the instance of this class
*/
class KeyBoardManager {

  private:
    KeyBoardManager();
    ~KeyBoardManager();
    KeyBoardManager(const KeyBoardManager&);
    KeyBoardManager& operator = (const KeyBoardManager&);
    KeyInfo keyInfo_[MAX_KEY_BUF];
    int keyBufIndex_;
    int keyBufGottenIndex_;
    bool isSpecialKey_;
    bool isKeyboardId_;
    static const int keyMap_[128];
  public:

    static KeyBoardManager& instance() {
        static KeyBoardManager theInstance;
        return theInstance;
    }

    KeyInfo* getKeyInfo();
    void setKeyScanCode(unsigned char);
    void printInfo() const;
};


#endif
