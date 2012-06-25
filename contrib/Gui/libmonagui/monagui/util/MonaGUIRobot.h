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

#ifndef _MONAGUI_ROBOT_
#define _MONAGUI_ROBOT_

#include <monapi/Robot.h>

namespace monagui {

class MonaGUIRobot : public MonAPI::Robot
{
public:
    void click(Component& button)
    {
        Rectangle r = *(button.getBounds());
        Frame* parent = (Frame*)button.getParent();
        MONA_ASSERT(parent);
        r.setX(r.getX() + parent->getBounds()->getX() + parent->getInsets()->left);
        r.setY(r.getY() + parent->getBounds()->getY() + parent->getInsets()->top);
        mouseMove(r.getX(), r.getY());
        sleep(100);
        mousePress();
        mouseRelease();
    }

    void input(Component& input, const std::string& text)
    {
        Rectangle r = *(input.getBounds());
        Frame* parent = (Frame*)input.getParent();
        MONA_ASSERT(parent);
        r.setX(r.getX() + parent->getBounds()->getX() + parent->getInsets()->left);
        r.setY(r.getY() + parent->getBounds()->getY() + parent->getInsets()->top);
        mouseMove(r.getX(), r.getY());
        sleep(100);
        mousePress();
        mouseRelease();
        type(text.c_str());
    }
};

};

#endif // _MONAGUI_ROBOT_
