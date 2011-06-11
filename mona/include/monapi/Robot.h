/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MONAPI_ROBOT_
#define _MONAPI_ROBOT_

namespace MonAPI {

class Robot
{
private:
    uint32_t mouseServer_;
    uint32_t keyboardServer_;

    void doKeyboardAction(uint32_t actionHeader, uint32_t arg1)
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, keyboardServer_, actionHeader, arg1) != M_OK) {
            monapi_warn("keyboard server is dead?");
            return;
        }
    }

    void doMouseAction(uint32_t actionHeader, uint32_t arg1 = 0, uint32_t arg2 = 0)
    {
        MessageInfo msg;
        if (Message::sendReceive(&msg, mouseServer_, actionHeader, arg1, arg2) != M_OK) {
            monapi_warn("mouse server is dead?");
            return;
        }
    }

public:
    Robot()
    {
        if (monapi_name_whereis("/servers/mouse", mouseServer_) != M_OK) {
            monapi_fatal("mouse server not found");
        }
        if (monapi_name_whereis("/servers/keyboard", keyboardServer_) != M_OK) {
            monapi_fatal("keyboard server not found");
        }
    }

    void mouseMove(int x, int y)
    {
        doMouseAction(MSG_MOUSE_SET_CURSOR_POSITION, x, y);
    }

    void keyPress(int keycode)
    {
        doKeyboardAction(MSG_KEY_PRESS, keycode);
    }

    void keyRelease(int keycode)
    {
        doKeyboardAction(MSG_KEY_RELEASE, keycode);
    }

    void mousePress()
    {
        doMouseAction(MSG_MOUSE_PRESS);
    }

    void mouseRelease()
    {
        doMouseAction(MSG_MOUSE_RELEASE);
    }

    void type(const char* text)
    {
        int len = strlen(text);
        for (int i = 0; i < len ;i++) {
            keyPress(text[i]);
        }
    }
};

};

#endif // _MONAPI_ROBOT_
