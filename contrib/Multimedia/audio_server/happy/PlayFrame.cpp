/*!
    \file   PlayFrame.cpp
    \brief  happy music player

    Copyright (c) 2007 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
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
}

PlayFrame::~PlayFrame()
{
}

void PlayFrame::repaint()
{
    if (this->__g == NULL) return;

    int w = getWidth();
    int h = getHeight();

    __g->setColor(getBackground());
    __g->fillRect(0, 0, w, h);

#define BORDER_COLOR Color::gray
#define TRANSPARENCE 0

    //  □□□□□□■
    //  □□□□■■□
    //  □□■■□□□
    //  □□■□□□□
    //  □■□□□□□
    //  □■□□□□□
    //  ■□□□□□□
    __g->drawPixel(0, 6, BORDER_COLOR);
    __g->drawPixel(1, 5, BORDER_COLOR);
    __g->drawPixel(1, 4, BORDER_COLOR);
    __g->drawPixel(2, 3, BORDER_COLOR);
    __g->drawPixel(2, 2, BORDER_COLOR);
    __g->drawPixel(3, 2, BORDER_COLOR);
    __g->drawPixel(4, 1, BORDER_COLOR);
    __g->drawPixel(5, 1, BORDER_COLOR);
    __g->drawPixel(6, 0, BORDER_COLOR);
    __g->setColor(TRANSPARENCE);
    __g->drawLine(0, 0, 5, 0);
    __g->drawLine(0, 1, 3, 1);
    __g->drawLine(0, 2, 1, 2);
    __g->drawLine(0, 3, 1, 3);
    __g->drawLine(0, 4, 0, 5);

    //  ■□□□□□□
    //  □■■□□□□
    //  □□□■■□□
    //  □□□□■□□
    //  □□□□□■□
    //  □□□□□■□
    //  □□□□□□■
    __g->drawPixel(w - 7, 0, BORDER_COLOR);
    __g->drawPixel(w - 6, 1, BORDER_COLOR);
    __g->drawPixel(w - 5, 1, BORDER_COLOR);
    __g->drawPixel(w - 4, 2, BORDER_COLOR);
    __g->drawPixel(w - 3, 2, BORDER_COLOR);
    __g->drawPixel(w - 3, 3, BORDER_COLOR);
    __g->drawPixel(w - 2, 4, BORDER_COLOR);
    __g->drawPixel(w - 2, 5, BORDER_COLOR);
    __g->drawPixel(w - 1, 6, BORDER_COLOR);
    __g->setColor(TRANSPARENCE);
    __g->drawLine(w - 6, 0, w - 1, 0);
    __g->drawLine(w - 4, 1, w - 1, 1);
    __g->drawLine(w - 2, 2, w - 1, 2);
    __g->drawLine(w - 2, 3, w - 1, 3);
    __g->drawLine(w - 1, 4, w - 1, 5);

    //  ■□□□□□□
    //  □■□□□□□
    //  □■□□□□□
    //  □□■□□□□
    //  □□■■□□□
    //  □□□□■■□
    //  □□□□□□■
    __g->drawPixel(6, h - 1, BORDER_COLOR);
    __g->drawPixel(5, h - 2, BORDER_COLOR);
    __g->drawPixel(4, h - 2, BORDER_COLOR);
    __g->drawPixel(3, h - 3, BORDER_COLOR);
    __g->drawPixel(2, h - 3, BORDER_COLOR);
    __g->drawPixel(2, h - 4, BORDER_COLOR);
    __g->drawPixel(1, h - 5, BORDER_COLOR);
    __g->drawPixel(1, h - 6, BORDER_COLOR);
    __g->drawPixel(0, h - 7, BORDER_COLOR);
    __g->setColor(TRANSPARENCE);
    __g->drawLine(0, h - 1, 5, h - 1);
    __g->drawLine(0, h - 2, 3, h - 2);
    __g->drawLine(0, h - 3, 1, h - 3);
    __g->drawLine(0, h - 4, 1, h - 4);
    __g->drawLine(0, h - 5, 0, h - 6);

    //  □□□□□□■
    //  □□□□□■□
    //  □□□□□■□
    //  □□□□■□□
    //  □□□■■□□
    //  □■■□□□□
    //  ■□□□□□□
    __g->drawPixel(w - 1, h - 7, BORDER_COLOR);
    __g->drawPixel(w - 2, h - 6, BORDER_COLOR);
    __g->drawPixel(w - 2, h - 5, BORDER_COLOR);
    __g->drawPixel(w - 3, h - 4, BORDER_COLOR);
    __g->drawPixel(w - 3, h - 3, BORDER_COLOR);
    __g->drawPixel(w - 4, h - 3, BORDER_COLOR);
    __g->drawPixel(w - 5, h - 2, BORDER_COLOR);
    __g->drawPixel(w - 6, h - 2, BORDER_COLOR);
    __g->drawPixel(w - 7, h - 1, BORDER_COLOR);
    __g->setColor(TRANSPARENCE);
    __g->drawLine(w - 1, h - 1, w - 6, h - 1);
    __g->drawLine(w - 1, h - 2, w - 4, h - 2);
    __g->drawLine(w - 1, h - 3, w - 2, h - 3);
    __g->drawLine(w - 1, h - 4, w - 2, h - 4);
    __g->drawLine(w - 1, h - 5, w - 1, h - 6);

    // close button
    __g->setColor(Color::gray);
    __g->drawLine(w - 17, 6, w - 12, 11);
    __g->drawLine(w - 12, 6, w - 17, 11);

//    __g->drawImage(icon, 10, 2);
    Container::repaint();
}

} // namespace baygui
