/* Kukuri for Mona
 * 
 * Copyright (c) 2000 h-muro
 * Copyright (c) 2000, 2004 Tino
 * Copyright (c) 2004 Higepon
 * 
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// This file's encoding is UTF-8.

#include "Kukuri.h"
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/Cursor.h>
#include <gui/System/Drawing/Bitmap.h>
#include <gui/messages.h>
#include <monapi.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern dword __gui_server;
extern _P<MonAPI::Screen> GetDefaultScreen();

class Form1 : public Form
{
private:

    _P<Timer> timer;
    _P<Bitmap> kukuri;

public:
    Form1()
    {
        this->offset = Point::get_Empty();
        this->InitializeComponent();
        _P<MonAPI::Screen> scr = GetDefaultScreen();
        this->set_Location(Point(
            (scr->getWidth () - KUKURI_WIDTH ) / 2,
            (scr->getHeight() - KUKURI_HEIGHT) / 2));

        String bundlePath = MonAPI::System::getBundlePath();
        this->kukuri = new Bitmap(bundlePath + "/KUKURI.BM2");
    }

    virtual void Create()
    {
        Form::Create();
        this->timer->Start();
    }

    virtual void Dispose()
    {
        this->timer->Dispose();
        this->kukuri->Dispose();
        Form::Dispose();
    }

private:
    void InitializeComponent()
    {
        this->set_ClientSize(Size(KUKURI_WIDTH, KUKURI_HEIGHT));
        this->set_TransparencyKey(KUKURI_NONE);

        this->timer = new Timer();
        this->timer->set_Interval((int)(SPEED * 1000));
        this->timer->add_Tick(new EventHandler<Form1>(this, &Form1::TimerHandler));
    }

    void TimerHandler(_P<Object> sender, _P<EventArgs> e)
    {
        // 最前面に移動
        MonAPI::Message::send(__gui_server, MSG_GUISERVER_WINDOWTOFRONTMOST, this->get_Handle());

        this->calculation(Cursor::get_Position());
    }

    void calculation(Point pt)
    {
        double dx = pt.X - (this->get_X() + this->get_Width() / 2);
        double dy = pt.Y - (this->get_Y() + this->get_Height() / 2);
        double distance = sqrt(dx * dx + dy * dy);
        if (distance < 16) return;

        // 角度（ラジアン）
        double rr = atan2(dx, dy);
        //r = int(DEG(rr));

        this->set_Location(Point(
            this->get_X() + (int)(IDOU * cos(rr)),
            this->get_Y() + (int)(IDOU * sin(rr))));
    }

protected:
    virtual void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();
        g->DrawImage(this->kukuri, 0, 0 - KUKURI_HEIGHT * kukuri_f);
        g->Dispose();
    }

    virtual NCState NCHitTest(int x, int y)
    {
        return NCState_TitleBar;
    }

    virtual void OnNCMouseDown(_P<MouseEventArgs> e)
    {
        if (e->Button != 2) Form::OnNCMouseDown(e);
    }

    virtual void OnNCMouseUp(_P<MouseEventArgs> e)
    {
        int x = e->X + this->offset.X, y = e->Y + this->offset.Y;
        if (e->Button == 2 && 0 <= x && x < this->get_Width() && 0 <= y && y < this->get_Height())
        {
            this->Dispose();
        }
        else
        {
            Form::OnNCMouseUp(e);
        }
    }

public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
