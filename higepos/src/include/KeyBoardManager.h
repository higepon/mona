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

#include<higepostypes.h>

#define MAX_KEY_BUF 255

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
    KeyInfo keyInfo[MAX_KEY_BUF];
  public:

    static KeyBoardManager& instance() {
        static KeyBoardManager theInstance;
        return theInstance;
    }

    KeyInfo* getKeyInfo();
    void setKeyInfo();
};


#endif
