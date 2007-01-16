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
#include <monapi/Keys.h>
//#include "KeyMaps.h"

#define MAX_KEY_BUF  255
#define SPECIAL_KEY  0xE0
#define KEYBOARD_ACK 0xFA

/*!
    keyboard input management  class
*/
class KeyBoardManager{

  public:
    KeyBoardManager();
    ~KeyBoardManager();
    KeyBoardManager(const KeyBoardManager&);
    KeyBoardManager& operator = (const KeyBoardManager&);
    KeyInfo* getKeyInfo(KeyInfo* keyinfo);
    int setKeyScanCode(unsigned char);
    void init();
    void toVirtual(uint8_t keycode, uint8_t modifiers, KeyInfo* info);
    inline bool isInit() const {
        return isInit_;
    }
    bool SetKeyMap(MonAPI::Keys::KeyboardType basicKeyMap);
    bool SetKeyMap(const int *customKeyMap, const int *customKeyMapE0);
    bool SetKeyTopMap(MonAPI::Keys::KeyboardType basicKeyTopMap);
    bool SetKeyTopMap(const char *customKeyTopMap, const char *customKeyTopMapS);

  private:
    bool isSpecialKey_;
    bool isKeyboardId_;
    bool isShift_;
    bool isAlt_;
    bool isCtrl_;
    bool isWin_;
    bool isMenu_;
    bool isInit_;
    int keyMap[128];
    int keyMapE0[128];
    char keyTopMap[256];
    char keyTopMapS[256];
    uint8_t idHigh_;
    uint8_t idLow_;
    List<KeyInfo*>* keyInfoList_;

};
#endif
