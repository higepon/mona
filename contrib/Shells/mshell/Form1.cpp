/*!
    \file   Form1.cpp
    \brief  mshell for MonaForms

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author  Tino, Higepon
    \version $Revision$
    \date   create: update:$Date$
*/

#include <monapi.h>
#include <monapi/CString.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/ControlPaint.h>

#define FONT_WIDTH    6
#define FONT_HEIGHT  12
#define SHELL_WIDTH  80
#define SHELL_HEIGHT 25

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern _P<MonAPI::Screen> GetDefaultScreen();

class Terminal : public Control
{
public:
    Terminal() : x(0), y(0)
    {
        this->set_BackColor(Color::get_White());
        this->offset = Point(2, 2);
    }

    virtual ~Terminal() {}

    virtual void OnPaint()
    {
        _P<Graphics> g = Graphics::FromImage(this->buffer);
        ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
        g->Dispose();
    }

    MonAPI::CString filterJapanize(MonAPI::CString s)
    {
        // iikagen filter
        if (s.indexOf("Can not find command") != -1)
        {
            return "\nコマンドが見つかりません\n";
        }
        else
        {
            return s;
        }
    }

    MonAPI::CString filterRemoveEscapeSequence(MonAPI::CString s)
    {
        MonAPI::CString ret;
        uint32_t length = s.getLength();
        bool inEscapeSequence = false;
        for (uint32_t i = 0; i < length; i++)
        {
            char c = s[i];
            if (c == 0x1b)
            {
                inEscapeSequence = true;
                continue;
            }

            if (!inEscapeSequence)
            {
                ret += c;
                continue;
            }

            if ((('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')))
            {
                if (i + 1 < length && s[i] == 'm' && s[i + 1] == ' ')
                {
                    i++;
                }
                inEscapeSequence = false;
            }
        }
        return ret;
    }

    void Output(MonAPI::CString s)
    {
        s = filterRemoveEscapeSequence(s);
        s = filterJapanize(s);
        OutputToTerminal((const char*)s);
    }

    void OutputToTerminal(String s)
    {
        _P<Graphics> g = this->CreateGraphics();
        Size r = this->get_ClientSize();
        wchar prev = 0;
        FOREACH (wchar, ch, s)
        {
            if (ch == '\r' || ch == '\n')
            {
                if (prev == '\r' && ch == '\n') continue;
                x = 0;
                if (y <= r.Height - FONT_HEIGHT * 2)
                {
                    y += FONT_HEIGHT;
                }
                else
                {
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
                    x = 0;
                    g->FillRectangle(this->get_BackColor(), 0, y, r.Width, r.Height - y);
                }
            }
            else
            {
                _A<wchar> buf(1);
                buf[0] = ch;
                int sw = g->MeasureString(buf, Control::get_DefaultFont()).Width;
                int sw2 = ch < 256 ? FONT_WIDTH : FONT_WIDTH * 2;
                g->DrawString(buf, Control::get_DefaultFont(), Color::get_Black(), x + (sw2 - sw) / 2, y);
                x += sw2;
            }
            prev = ch;
        }
        END_FOREACH
        g->Dispose();
    }

private:
    int x, y;
};

static uint32_t my_tid, stdout_tid;
static _P<Terminal> terminal;

#if 0
static void StdoutMessageLoop()
{
    MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);

    for (MessageInfo msg;;)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
            case MSG_PROCESS_STDOUT_DATA:
            {
                msg.str[127] = '\0';
                terminal->Output(msg.str);
                terminal->Refresh();
                MonAPI::Message::reply(&msg);
                break;
            }
        }
    }
}
#else

static uint32_t oldStreamOutHandle;

static void StdoutMessageLoop()
{
    MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);
    MessageInfo msg;
    uint32_t targetID = MonAPI::Message::lookupMainThread("SHELL.EX5");
    MonAPI::Stream stream;
    MonAPI::Message::sendReceive(&msg, targetID, MSG_CHANGE_OUT_STREAM_BY_HANDLE, stream.handle());
    oldStreamOutHandle = msg.arg2;
    const uint32_t BUFFER_SIZE = 256;
    for (;;)
    {
        uint8_t buffer[BUFFER_SIZE];
        stream.waitForRead();
        uint32_t size = stream.read(buffer, BUFFER_SIZE);
        buffer[size == BUFFER_SIZE ? BUFFER_SIZE - 1 : size] = '\0';
        terminal->Output((char*)buffer);
        terminal->Refresh();
    }
}
#endif


static void InitThread()
{
    my_tid = syscall_get_tid();
    uint32_t id = syscall_mthread_create((uint32_t)StdoutMessageLoop);
// comment out by higepon
//    syscall_mthread_join(id);
    MessageInfo msg, src;
    src.header = MSG_SERVER_START_OK;
    MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
    stdout_tid = msg.from;
}

class Form1 : public Form
{
private:
    uint32_t shell;

public:
    Form1()
    {
        this->InitializeComponent();

        this->shell = MonAPI::Message::lookupMainThread("SHELL.EX5");

        _P<MonAPI::Screen> scr = GetDefaultScreen();
        this->set_Location(Point((scr->getWidth() - this->get_Width()) / 2, (scr->getHeight() - this->get_Height()) / 2));

        terminal = new Terminal();
        terminal->set_Bounds(Rectangle(Point::get_Empty(), this->get_ClientSize()));
        this->get_Controls()->Add(terminal.get());
    }

protected:
    virtual void OnKeyDown(_P<KeyEventArgs> e)
    {
        Form::OnKeyDown(e);
        if (this->shell == THREAD_UNKNOWN) return;

        MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, e->KeyCode, e->Modifiers);
    }

private:
    void InitializeComponent()
    {
        this->set_ClientSize(Size(SHELL_WIDTH * FONT_WIDTH + 4, SHELL_HEIGHT * FONT_HEIGHT + 4));
        this->set_Text("GUI Shell");
        this->set_Opacity(0.8);
    }

public:
    static void Main(_A<String> args)
    {
        _P<Form1> f = new Form1();
        InitThread();
        f->Show();
        if (f->shell != THREAD_UNKNOWN) MonAPI::Message::send(f->shell, MSG_GUISERVER_KEYDOWN, 0, '\r');
        Application::Run(f.get());
        syscall_kill_thread(stdout_tid);
        MessageInfo msg;
        MonAPI::Message::sendReceive(&msg, f->shell, MSG_CHANGE_OUT_STREAM_BY_HANDLE, oldStreamOutHandle);
    }
};

SET_MAIN_CLASS(Form1)
