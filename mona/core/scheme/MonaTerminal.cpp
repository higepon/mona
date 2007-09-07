/*!
    \file   MonaTerminal.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#ifdef MONA

#include "MonaTerminal.h"
#include <monapi.h>
#include <monapi/messages.h>
#include <servers/screen.h>

using namespace util;
using namespace monash;
using namespace MonAPI;

MonaTerminal::MonaTerminal() : isKeySuppressed_(false), line_(""), cursorPosition_(0)
{
    if (!initialize()) return;
}

MonaTerminal::~MonaTerminal()
{
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);
}

::util::String MonaTerminal::getLine()
{
    bool hasLine = false;
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;
        switch (msg.header)
        {
            case MSG_KEY_VIRTUAL_CODE:

                if (!isKeySuppressed_ && (msg.arg2 & KEY_MODIFIER_DOWN) != 0)
                {
                    hasLine = onKeyDown(msg.arg1, msg.arg2);
                }
                else if (msg.arg1 == 0)
                {
                    hasLine = onKeyDown(msg.arg2, msg.arg3);
                }
                if (hasLine)
                {
                    String ret = line_;
                    line_ = "";
                    cursorPosition_ = 0;
                    return ret;
                }
                break;
            case MSG_CHANGE_OUT_STREAM_BY_HANDLE:
            {
                MessageInfo m;
                uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
                if (targetID == THREAD_UNKNOWN || Message::sendReceive(&m, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
                    MONAPI_WARN("SCREEN.EX5 not found");
                    continue;
                }
                screenHandle_ = msg.arg1;
                uint32_t s = m.arg2;
                terminal_ = new terminal::Util(Stream::FromHandle(msg.arg1));
                Message::reply(&msg, s);
                break;
            }

        default:
                break;
        }
    }
    return "";
}

int MonaTerminal::formatWrite(const char* format, ...)
{
    char str[512];
    str[0] = '\0';
    va_list args;
    int result;
    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > 512)
    {
        _printf("Shell::out:overflow");
    }
    terminal_->write(str);
    return terminal_->flush();
}



/*----------------------------------------------------------------------
    private functions
----------------------------------------------------------------------*/

bool MonaTerminal::initialize()
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1))
    {
        fprintf(stderr, "register to keyboard server failed\n");
        return false;
    }

    initHistories();
//     histories_.add("(load \"/LIBS/SCHEME/fib.scm\")");
//     histories_.add("(call-process-out-string \"/APPS/HELLO.EX5\")");
//     histories_.add("(call-process \"/APPS/MONAFRMS/BITMAP.EX5 /APPS/MONAFRMS/MONALISA.JPG\")");
//     histories_.add("(| \"/APPS/HELLO.EX5\" \"/APPS/UPPER.EX5\" \"/APPS/REVERSE.EX5\")");
//     histories_.add("(exec \"/LIBS/SCHEME/fib.scm\")");
//     histories_.add("(for-each (lambda (w) (print (mona.gui.window-title w))) (filter #/APPS/ (mona.gui.enum-windows)))");


    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1))
    {
        fprintf(stderr, "register to process server failed\n");
        return false;
    }

    MessageInfo msg;
    uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
    if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
        fprintf(stderr, "SCREEN.EX5 not found\n");
        return false;
    }
    screenHandle_ = msg.arg2;
    terminal_ = new terminal::Util(Stream::FromHandle(screenHandle_));
    return true;

}

bool MonaTerminal::onKeyDown(int keycode, int modifiers)
{
    switch(keycode) {
    case (Keys::H):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            backSpace();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case(Keys::Up):
        outputLine(histories_.getOlderHistory()->data());
        break;
    case(Keys::Down):
        outputLine(histories_.getNewerHistory()->data());
        break;

    case (Keys::P):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            outputLine(histories_.getOlderHistory()->data());
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::N):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            outputLine(histories_.getNewerHistory()->data());
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case(Keys::Left):
        cursorBackward();
        break;
    case (Keys::B):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            cursorBackward();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::A):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            cursorBeginingOfLine();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::E):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            cursorEndOfLine();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case(Keys::Right):
        cursorForward();
        break;
    case (Keys::F):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            cursorForward();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::K):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            killLine();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::D):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            del();
            break;
        }
        else
        {
            outputKey(keycode, modifiers);
            break;
        }
    case (Keys::C):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            reEdit();
            break;
        }
       else
        {
            outputKey(keycode, modifiers);
            break;
        }

    case(Keys::G):
    case(Keys::I):case(Keys::J):case(Keys::L):
    case(Keys::M):case(Keys::O):
    case(Keys::Q):case(Keys::R):case(Keys::S):case(Keys::T):
    case(Keys::U):case(Keys::V):case(Keys::W):case(Keys::X):
    case(Keys::Y):case(Keys::Z):case(Keys::Decimal):case(Keys::D0):
    case(Keys::D1):case(Keys::D2):case(Keys::D3):case(Keys::D4):
    case(Keys::D5):case(Keys::D6):case(Keys::D7):case(Keys::D8):
    case(Keys::D9):case(Keys::NumPad1):case(Keys::NumPad2):case(Keys::NumPad3):
    case(Keys::NumPad4):case(Keys::NumPad5):case(Keys::NumPad6):case(Keys::NumPad7):
    case(Keys::NumPad8):case(Keys::NumPad9):case(Keys::NumPad0):case(Keys::Subtract):
    case(Keys::Add):case(Keys::Space):case(Keys::Divide):case(Keys::OemPeriod):
    case(Keys::OemPipe):case(Keys::OemQuestion):case(Keys::OemMinus):case(Keys::OemBackslash):
    case(Keys::OemSemicolon):case(Keys::Oemplus):
        outputKey(keycode, modifiers);
        break;
    case(Keys::Enter):
        line_ += '\n';
        return true;
        break;

    case(Keys::Back):
        backSpace();
        break;
    default:
        break;
    }
    return false;
}

void MonaTerminal::outputKey(int keycode, int modifiers)
{
    KeyInfo key;
    key.keycode = keycode;
    key.modifiers = modifiers;
    outputChar(Keys::ToChar(key));
}

void MonaTerminal::backSpace()
{
    if (cursorPosition_ == 0) return;
    terminal_->moveCursorLeft(cursorPosition_);
    cursorPosition_--;
    line_.removeAt(cursorPosition_);

    terminal_->write(line_.data());
    terminal_->write(" ");
    terminal_->moveCursorLeft(line_.size() - cursorPosition_ + 1);
    terminal_->flush();
}

void MonaTerminal::reEdit()
{
    formatWrite("\n");
    line_ += '\n';
    cursorPosition_ = 0;
    line_ = "";
}

void MonaTerminal::cursorBackward(int n /* = 1 */)
{
    if (cursorPosition_ == 0) return;
    if ((int)cursorPosition_ < n)
    {
        n = cursorPosition_;
    }
    cursorPosition_ -= n;
    terminal_->moveCursorLeft(n);
    terminal_->flush();
}

void MonaTerminal::cursorForward(int n /* = 1 */)
{
    if (cursorPosition_ == line_.size()) return;
    if (cursorPosition_ + n > line_.size())
    {
        n = line_.size() - cursorPosition_;
    }
    cursorPosition_ += n;
    terminal_->moveCursorRight(n);
    terminal_->flush();
}

void MonaTerminal::del()
{
    if (line_.size() == cursorPosition_) return;
    for (uint32_t i = 0; i < cursorPosition_; i++)
    {
        terminal_->backSpace();
    }
    line_.removeAt(cursorPosition_);

    terminal_->write(line_.data());
    terminal_->write(" ");
    for (uint32_t i = 0; i < line_.size() - cursorPosition_ + 1; i++)
    {
        terminal_->backSpace();
    }
    terminal_->flush();
}

void MonaTerminal::cursorBeginingOfLine()
{
    terminal_->moveCursorLeft(cursorPosition_);
    cursorPosition_ = 0;
    terminal_->flush();
}

void MonaTerminal::cursorEndOfLine()
{
    terminal_->write(line_.substring(cursorPosition_, line_.size() - cursorPosition_).data());
    cursorPosition_ = line_.size();
    terminal_->flush();
}

void MonaTerminal::killLine()
{
    uint32_t times = line_.size() - cursorPosition_;
    line_ = line_.substring(0, cursorPosition_);
    cursorPosition_ = line_.size();
    for (uint32_t i = 0; i < times; i++)
    {
        terminal_->write(" ");
    }

    terminal_->moveCursorLeft(times);
    terminal_->flush();
}

void MonaTerminal::outputLine(::util::String l)
{
    cursorBeginingOfLine();
    killLine();
    line_ = l;
    formatWrite("%s", line_.data());
    cursorPosition_ = line_.size();
}

void MonaTerminal::outputChar(char c)
{
    line_.insert(cursorPosition_, c);
    formatWrite("%c", c);

    if (cursorPosition_ != line_.size() - 1)
    {
        formatWrite("%s", line_.substring(cursorPosition_ + 1, line_.size() - cursorPosition_).data());
        terminal_->moveCursorLeft(line_.size() - cursorPosition_ - 1);
    }
    terminal_->flush();
    cursorPosition_++;
}

#endif // MONA
