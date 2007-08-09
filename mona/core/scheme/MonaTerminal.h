/*!
    \file  MonaTerminal.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __MONA_TERMINAL_H__
#define __MONA_TERMINAL_H__

#ifdef MONA

#include "CommandHistory.h"
#include <monapi/terminal/Util.h>

namespace monash {

class MonaTerminal
{
public:
    MonaTerminal(void(*timerFunc)(void));
    virtual ~MonaTerminal();

    ::util::String getLine();
    int formatWrite(const char* format, ...);
    uint32_t getScreenHandle() const { return screenHandle_; }
    void setKeySuppresed() { isKeySuppressed_ = true; }
    void outputChar(char c);
protected:
    bool initialize();
    bool onKeyDown(int keycode, int modifiers);
    void outputKey(int keycode, int modifiers);
    void backSpace();
    void reEdit();
    void cursorBackward(int n = 1);
    void cursorForward(int n = 1);
    void del();
    void cursorBeginingOfLine();
    void cursorEndOfLine();
    void killLine();
    void outputLine(::util::String l);
    void initHistories();

    bool isKeySuppressed_;
    void(*timerFunc_)(void);
    ::util::String line_;
    uint32_t cursorPosition_;
    CommandHistory histories_;
    uint32_t screenHandle_;
    ::MonAPI::terminal::Util* terminal_;
};



}; // namespace monash



#endif // __MONA_TERMINAL_H__

#endif // MONA
