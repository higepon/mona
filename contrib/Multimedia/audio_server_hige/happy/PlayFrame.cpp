/*!
    \file   PlayFrame.cpp
    \brief  happy music player

    Copyright (c) 2007 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2007/09/26 update:$Date$
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
//    icon = new Image(iconPath);
}

PlayFrame::~PlayFrame()
{
//    delete icon;
}

void PlayFrame::repaint()
{
    if (this->__g == NULL) return;

    int w = getWidth();
    int h = getHeight();

    __g->setColor(0xff, 0xff, 0xff);
    __g->fillRect(0, 0, w, h);

#define BORDER_COLOR Color::red
#define BORDER_COLOR2 Color::blue

#if 0
    __g->drawPixel(0, 0, BORDER_COLOR);
    __g->drawPixel(1, 0, BORDER_COLOR);
    __g->drawPixel(0, 1, BORDER_COLOR);

    __g->drawPixel(0, h - 2, BORDER_COLOR);
    __g->drawPixel(0, h - 1, BORDER_COLOR);
    __g->drawPixel(1, h - 1, BORDER_COLOR);

    __g->drawPixel(w - 1, 0, BORDER_COLOR);
    __g->drawPixel(w - 2, 0, BORDER_COLOR);
    __g->drawPixel(w - 1, 1, BORDER_COLOR);

    __g->drawPixel(w - 1, h - 1, BORDER_COLOR);
    __g->drawPixel(w - 2, h - 1, BORDER_COLOR);
    __g->drawPixel(w - 1, h - 2, BORDER_COLOR);
#endif

    __g->setColor(BORDER_COLOR2);
    __g->drawLine(1, 0, w - 2, 0);
    __g->drawLine(1, h - 1, w - 2, h - 1);
    __g->drawLine(0, 1, 0, h - 2);
    __g->drawLine(w - 1, 1, w - 1, h - 2);

    __g->setColor(BORDER_COLOR);
    __g->drawPixel(4, 0, BORDER_COLOR);
    __g->drawPixel(0, 4, BORDER_COLOR);
    __g->drawLine(1, 1, 1, 3);
    __g->drawLine(1, 1, 3, 1);




    // close button
    __g->setColor(Color::gray);
    __g->drawLine(w - 12, 6, w - 7, 11);
    __g->drawLine(w - 7, 6, w - 12, 11);

//    __g->drawImage(icon, 10, 2);
    Container::repaint();
}

} // namespace baygui
