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
#include <monapi/terminal/Writer.h>
#include <monapi/terminal/CommandParser.h>
#include <monapi/text/Line.h>
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

class Terminal : public Control, public MonAPI::terminal::Writer
{
public:
    Terminal() : x_(0), y_(0)
    {
        this->set_BackColor(Color::get_White());
        this->offset = Point(2, 2);
    }

    virtual ~Terminal() {}

    virtual int clearScreen()
    {
        return 0;
    }

    virtual int moveCursor(uint32_t x, uint32_t y)
    {
        return 0;
    }

    virtual int moveCursorUp(uint32_t n)
    {
        return 0;
    }

    virtual int moveCursorDown(uint32_t n)
    {
        return 0;
    }

    virtual int moveCursorLeft(uint32_t n)
    {
        for (uint32_t i = 0; i < n; i++)
        {
            line_.moveCursorLeft();
        }
        return 0;
    }
    virtual int moveCursorRight(uint32_t n)
    {
        for (uint32_t i = 0; i < n; i++)
        {
            line_.moveCursorRight();
        }
        return 0;
    }

    virtual int lineFeed()
    {
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
            Refresh();
        }
        return 0;
    }

    virtual int backSpace()
    {
        line_.moveCursorLeft();
        return 0;
    }

    virtual int write(uint8_t* buf, uint32_t length)
    {
        _P<Graphics> g = this->CreateGraphics();
        MonAPI::CString text((char*)buf, length);
        line_.write(text);
        Size r = this->get_ClientSize();
        g->FillRectangle(this->get_BackColor(), 0, y_, r.Width, FONT_HEIGHT);
        g->DrawString((const char*)(line_.get()), Control::get_DefaultFont(), Color::get_Black(), 0, y_);
        g->Dispose();
        Refresh();
        return 0;
    }
    virtual uint32_t getX() const { return x_; }
    virtual uint32_t getY() const { return y_; }

    virtual void OnPaint()
    {
        _P<Graphics> g = Graphics::FromImage(this->buffer);
        ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
        g->Dispose();
    }

private:
    int x_, y_;
    MonAPI::text::Line line_;
};

static uint32_t my_tid, stdout_tid;
static _P<Terminal> terminal;
static MonAPI::terminal::CommandParser* parser;
static uint32_t oldStreamOutHandle;

static void StdoutMessageLoop()
{
    MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);
    MessageInfo msg;
    uint32_t targetID = MonAPI::Message::lookupMainThread("SHELL.EX5");
    MonAPI::Stream stream;
    MonAPI::Message::sendReceive(&msg, targetID, MSG_CHANGE_OUT_STREAM_BY_HANDLE, stream.handle());
    oldStreamOutHandle = msg.arg2;
    parser = new MonAPI::terminal::CommandParser(terminal.get());
    const uint32_t BUFFER_SIZE = 256;
    for (;;)
    {
        uint8_t buffer[BUFFER_SIZE];
        stream.waitForRead();
        _logprintf("%s:%d\n", __func__, __LINE__);
        uint32_t size = stream.read(buffer, BUFFER_SIZE);
        _logprintf("%s:%d\n", __func__, __LINE__);
        if (size == 0) continue;
        parser->parse(buffer, size);
    }
}

static void InitThread()
{
        _logprintf("%s:%d\n", __func__, __LINE__);
    my_tid = syscall_get_tid();
        _logprintf("%s:%d\n", __func__, __LINE__);
    uint32_t id = syscall_mthread_create((uint32_t)StdoutMessageLoop);
        _logprintf("%s:%d\n", __func__, __LINE__);
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
        _logprintf("%s:%d\n", __func__, __LINE__);
        _P<Form1> f = new Form1();
        _logprintf("%s:%d\n", __func__, __LINE__);
        InitThread();
        _logprintf("%s:%d\n", __func__, __LINE__);
        f->Show();
        _logprintf("%s:%d\n", __func__, __LINE__);
        if (f->shell != THREAD_UNKNOWN) MonAPI::Message::send(f->shell, MSG_GUISERVER_KEYDOWN, 0, '\r');
        Application::Run(f.get());
        syscall_kill_thread(stdout_tid);
        MessageInfo msg;
        MonAPI::Message::sendReceive(&msg, f->shell, MSG_CHANGE_OUT_STREAM_BY_HANDLE, oldStreamOutHandle);
    }
};

SET_MAIN_CLASS(Form1)
