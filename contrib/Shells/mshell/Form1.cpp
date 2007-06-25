/*!
    \file   Form1.cpp
    \brief  mshell for MonaForms

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author  Tino, Higepon
    \version $Revision$
    \date   create: update:$Date$
*/

#include "Terminal.h"
#include <monapi/terminal/CommandParser.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern _P<MonAPI::Screen> GetDefaultScreen();
static _P<Terminal> g_terminal;
static uint32_t g_oldStreamOutHandle;

static void StdoutMessageLoop()
{
    MonAPI::Stream stream;
    MessageInfo msg;
    uint32_t shellServerID = monapi_get_server_thread_id(ID_SCHEME_SERVER);
    MonAPI::Message::sendReceive(&msg, shellServerID, MSG_CHANGE_OUT_STREAM_BY_HANDLE, stream.handle());
    g_oldStreamOutHandle = msg.arg2;

    MonAPI::terminal::CommandParser* parser = new MonAPI::terminal::CommandParser(g_terminal.get());
    const uint32_t BUFFER_SIZE = 256;
    for (;;)
    {
        uint8_t buffer[BUFFER_SIZE];
        stream.waitForRead();
        uint32_t size = stream.read(buffer, BUFFER_SIZE);
        if (size == 0) continue;
        // don't display ^EOP
        char* found = MonAPI::strstr_n((char*)buffer, "^EOP", size);
        if (NULL != found)
        {
            memset(found, '\0', 4);
        }

        parser->parse(buffer, size);
    }
}

class Form1 : public Form
{
private:
    uint32_t shell;
    bool first;

public:
    Form1() : first(true)
    {
        this->InitializeComponent();
        this->shell = monapi_get_server_thread_id(ID_SCHEME_SERVER);
        _P<MonAPI::Screen> scr = GetDefaultScreen();
        this->set_Location(Point((scr->getWidth() - this->get_Width()) / 2, (scr->getHeight() - this->get_Height()) / 2));
    }

    virtual void OnPaint()
    {
        // fix me, we need to force display prompt
        if (this->first)
        {
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::Enter, 0);
            // fix me
            // call (on-gui-start)
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::O, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::N, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::OemMinus, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::G, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::U, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::I, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::OemMinus, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::S, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::T, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::A, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::R, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::T, 0);
            MonAPI::Message::send(this->shell, MSG_KEY_VIRTUAL_CODE, 0, MonAPI::Keys::Enter, 0);

            this->first = false;
        }
        _P<Graphics> g = Graphics::FromImage(this->buffer);
        ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
        g->Dispose();
        Form::OnPaint();
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
        this->set_ClientSize(Size(Terminal::WIDTH * Terminal::FONT_WIDTH + 4, Terminal::HEIGHT * Terminal::FONT_HEIGHT + 4));
        this->set_Text("GUI Shell");
        this->set_Opacity(0.8);
        g_terminal = new Terminal();
        g_terminal->set_Bounds(Rectangle(Point::get_Empty(), this->get_ClientSize()));
        this->get_Controls()->Add(g_terminal.get());
    }

public:
    static void Main(_A<String> args)
    {
        _P<Form1> f = new Form1();
        uint32_t stdout_tid = syscall_mthread_create((uint32_t)StdoutMessageLoop);
        f->Show();
        if (f->shell != THREAD_UNKNOWN) MonAPI::Message::send(f->shell, MSG_GUISERVER_KEYDOWN, 0, '\r');
        Application::Run(f.get());
        syscall_kill_thread(stdout_tid);
        MessageInfo msg;
        MonAPI::Message::sendReceive(&msg, f->shell, MSG_CHANGE_OUT_STREAM_BY_HANDLE, g_oldStreamOutHandle);
    }
};

SET_MAIN_CLASS(Form1)
