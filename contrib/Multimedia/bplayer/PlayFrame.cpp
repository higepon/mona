/*!
    \file   PlayFrame.cpp
    \brief  beep music player

    Copyright (c) 2006 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2006/11/21 update:$Date$
*/

#include "PlayFrame.h"

namespace baygui
{
PlayFrame::PlayFrame(const char* iconPath)
{
    this->border        = 0;
    this->insets.top    = 15;
    this->insets.bottom = 6;
    this->insets.left   = 6;
    this->insets.right  = 6;
    icon = new Image(iconPath);
}

PlayFrame::~PlayFrame()
{
    delete icon;
}

void PlayFrame::repaint()
{
    if (this->__g == NULL) return;

    int w = getWidth();
    int h = getHeight();
    __g->setColor(33, 32, 33);
    __g->fillRect(0, 0, w, h);

    __g->drawPixel(0, 0, 0);
    __g->drawPixel(1, 0, 0);
    __g->drawPixel(0, 1, 0);

    __g->drawPixel(0, h - 2, 0);
    __g->drawPixel(0, h - 1, 0);
    __g->drawPixel(1, h - 1, 0);

    __g->drawPixel(w - 1, 0, 0);
    __g->drawPixel(w - 2, 0, 0);
    __g->drawPixel(w - 1, 1, 0);

    __g->drawPixel(w - 1, h - 1, 0);
    __g->drawPixel(w - 2, h - 1, 0);
    __g->drawPixel(w - 1, h - 2, 0);

    // close button
    __g->setColor(Color::gray);
    __g->drawLine(w - 12, 6, w - 7, 11);
    __g->drawLine(w - 7, 6, w - 12, 11);

    __g->drawImage(icon, 10, 2);
    Container::repaint();
}
}
