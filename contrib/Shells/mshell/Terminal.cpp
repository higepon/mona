/*!
    \file   Terminal.cpp
    \brief  mshell for MonaForms

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author  Higepon
    \version $Revision: 4278 $
    \date   create: 2007/05/12 update:$Date: 2007-05-11 22:35:49 +0900 (金, 11  5月 2007) $
*/

#include "Terminal.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

Terminal::Terminal() : x_(0), y_(0), first_(true)
{
    forgroundColor_  = Color::get_White();
    backgroundColor_ = Color::FromArgb(255, 7, 0, 53);
    set_BackColor(backgroundColor_);
    offset = Point(2, 2);
}

Terminal::~Terminal()
{
}

int Terminal::clearScreen()
{
    return 0;
}

int Terminal::moveCursor(uint32_t x, uint32_t y)
{
    return 0;
}

int Terminal::moveCursorUp(uint32_t n)
{
    return 0;
}

int Terminal::moveCursorDown(uint32_t n)
{
    return 0;
}

int Terminal::moveCursorLeft(uint32_t n)
{
    _P<Graphics> g = this->CreateGraphics();
    eraseCursor(g);
    line_.moveCursorLeft(n);
    drawLine(g);
    g->Dispose();
    return 0;
}

int Terminal::moveCursorRight(uint32_t n)
{
    _P<Graphics> g = this->CreateGraphics();
    eraseCursor(g);
    line_.moveCursorRight(n);
    drawLine(g);
    g->Dispose();
    return 0;
}

int Terminal::lineFeed()
{
    _P<Graphics> g = this->CreateGraphics();
    eraseCursor(g);

    line_.reset();
    Size r = this->get_ClientSize();
    if (y_ <= r.Height - FONT_HEIGHT * 2)
    {
        x_ = 0;
        y_ += FONT_HEIGHT;
    }
    else
    {
        _P<Graphics> g = this->CreateGraphics();
        Color* img = this->buffer->get();
        for (int yy = 0; yy < r.Height - FONT_HEIGHT; yy++)
        {
            int p = (yy + this->offset.Y) * this->get_Width() + this->offset.X;
            int d = this->get_Width() * FONT_HEIGHT;
            for (int xx = 0; xx < r.Width; xx++, p++)
            {
                img[p] = img[p + d];
            }
        }
        x_ = 0;
        g->FillRectangle(this->get_BackColor(), 0, y_, r.Width, r.Height - y_);
        g->Dispose();
    }
    drawLine(g);
    g->Dispose();
    return 0;
}

int Terminal::backSpace()
{
    _P<Graphics> g = this->CreateGraphics();
    eraseCursor(g);
    line_.moveCursorLeft();
    drawLine(g);
    g->Dispose();
    return 0;
}

int Terminal::write(uint8_t* buf, uint32_t length)
{
    _P<Graphics> g = this->CreateGraphics();
    eraseCursor(g);
    ::MonAPI::CString text((char*)buf, length);
    line_.write(text);
    Size r = this->get_ClientSize();
    drawLine(g);
    g->Dispose();
    return 0;
}

void Terminal::OnPaint()
{
    _P<Graphics> g = Graphics::FromImage(this->buffer);
    ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
    g->Dispose();
}

int Terminal::eraseCursor(_P<System::Drawing::Graphics> g)
{
    ::MonAPI::CString line = line_.get();
    ::MonAPI::CString linef = line.substring(0, line_.getCursorPosition());
    int cursorX = g->MeasureString((const char*)linef, Control::get_DefaultFont()).Width;
    g->FillRectangle(backgroundColor_, cursorX, y_ + 1, FONT_WIDTH, FONT_HEIGHT - 2);
    return 0;
}

int Terminal::drawLine(_P<System::Drawing::Graphics> g)
{
    if (line_.isCursorEndOfLine())
    {
        ::MonAPI::CString line = line_.get();
        int cursorX = g->MeasureString((const char*)line, Control::get_DefaultFont()).Width;
        g->DrawString((const char*)line, Control::get_DefaultFont(), forgroundColor_, 0, y_);
        g->FillRectangle(forgroundColor_, cursorX, y_ + 1, FONT_WIDTH, FONT_HEIGHT - 2);
    }
    else
    {
        Size r = this->get_ClientSize();
        g->FillRectangle(backgroundColor_, 0, y_, r.Width, FONT_HEIGHT);

        ::MonAPI::CString line = line_.get();
        ::MonAPI::CString forward  = line.substring(0, line_.getCursorPosition());
        ::MonAPI::CString current  = line.substring(line_.getCursorPosition(), 1);
        ::MonAPI::CString backword = line.substring(line_.getCursorPosition() + 1, line.getLength() - forward.getLength() - current.getLength());

        g->DrawString((const char*)forward, Control::get_DefaultFont(), forgroundColor_, 0, y_);

        int forwardLength = g->MeasureString((const char*)forward, Control::get_DefaultFont()).Width;
        int currentLength = g->MeasureString((const char*)current, Control::get_DefaultFont()).Width;
        g->FillRectangle(forgroundColor_, forwardLength, y_ + 1, currentLength, FONT_HEIGHT - 2);
        g->DrawString((const char*)current, Control::get_DefaultFont(), backgroundColor_, forwardLength, y_);
        g->DrawString((const char*)backword, Control::get_DefaultFont(), forgroundColor_, forwardLength + currentLength, y_);
    }
   return 0;
}
