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

#include "frame.h"

namespace facebook {

Frame::Frame(const std::string& title) : monagui::Frame(title.c_str())
{
}

void Frame::paint(Graphics *g)
{
    g->setColor(getBackground());
    g->fillRect(0, 0, getWidth(), getHeight());
    Graphics* frameGraphics = getFrameGraphics();
    paintTitleGradation(frameGraphics);
    drawCloseButton(frameGraphics);
    paintTitleString(frameGraphics);
}

void Frame::paintTitleGradation(Graphics* g)
{
    int w = getWidth();
    dword c = g->getColor();
    g->setColor(0x29, 0x3e, 0x6a);
    g->fillRect(1, 0, w - 2, 1);
    g->setColor(0x6c, 0x83, 0xb2);
    g->fillRect(1, 1, w - 2, 1);
    g->setColor(0x3b, 0x59, 0x98);
    g->fillRect(1, 2, w - 2, 16);
    g->setColor(0x62, 0x7a, 0xad);
    g->fillRect(1, 18, w - 2, 2);
    g->setColor(0x89, 0x9b, 0xc1);
    g->fillRect(0, 20, w - 2, 1);
    g->setColor(0x29, 0x3e, 0x6b);
    g->fillRect(0, 21, w - 2, 1);
    g->setColor(c);
}

void Frame::paintTitleString(Graphics* g)
{
    int w = getWidth();
    int fw = getFontMetrics()->getWidth(getTitle());
    int fh = getFontMetrics()->getHeight(getTitle());

    if (getFocused()) {
        g->setColor(monagui::Color::white);
    } else {
        g->setColor(monagui::Color::gray);
    }
    g->setFontStyle(Font::BOLD);
    g->drawString(getTitle(), ((w - fw) / 2), ((getInsets()->top - fh) / 2));
}

}
