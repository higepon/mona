// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.
// Higepon 2004

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <gui/System/Drawing/Font.h>
#include <monapi.h>
#include <monapi/messages.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

static int g_x;
static int g_y;
static int g_loop_tid;

static void MouseInfoLoop()
{
    g_loop_tid = MonAPI::System::getThreadID();

//      MessageInfo msg;
//      MonAPI::Message::create(&msg, MSG_REGISTER_TO_SERVER, g_loop_tid);
//      printf("id = %d %d", MonAPI::Message::lookupMainThread("MOUSE.SVR"), g_loop_tid);
//      MonAPI::Message::send(MonAPI::Message::lookupMainThread("MOUSE.SVR"), &msg);

//     printf("%d", MonAPI::Message::lookupMainThread("MOUSE.SVR"));
//      MonAPI::Message::sendReceive(NULL, MonAPI::Message::lookupMainThread("MOUSE.SVR"), MSG_REGISTER_TO_SERVER, g_loop_tid, g_loop_tid);

    monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_TRUE);

    MessageInfo receive;

    for (;;)
    {
        if (MonAPI::Message::receive(&receive) != 0) continue;

        switch(receive.header)
        {
        case MSG_MOUSE_INFO:

            g_x = (int)receive.arg1;
            g_y = (int)receive.arg2;
            break;
        }
    }
}

class Form1 : public Form
{
private:
    _P<Timer> timer;
    int x;
    int y;
    double angle;

public:
    Form1()
    {
        x = 50;
        y = 50;

        this->InitializeComponent();

        dword id = syscall_mthread_create((dword)MouseInfoLoop);
        syscall_mthread_join(id);
    }

    virtual void Create()
    {
        Form::Create();
        this->timer->Start();
    }

    virtual void Dispose()
    {
//      MessageInfo msg;
//      MonAPI::Message::create(&msg, MSG_UNREGISTER_FROM_SERVER, g_loop_tid);
//      MonAPI::Message::send(MonAPI::Message::lookupMainThread("MOUSE.SVR"), &msg);

        /* kill message loop thread */
        MonAPI::System::kill(g_loop_tid);

        this->timer->Dispose();
        Form::Dispose();
    }

private:
    void InitializeComponent()
    {
        this->set_Location(Point(x, y));
        this->set_ClientSize(Size(150, 50));
        this->set_BackColor(Color::get_Empty());

        this->set_Text("???");

        this->timer = new Timer();
        this->timer->set_Interval(100);
        this->timer->add_Tick(new EventHandler<Form1>(this, &Form1::TimerHandler));
    }

    void TimerHandler(_P<Object> sender, _P<EventArgs> e)
    {
        double v = 10;
        double distance = sqrt((g_x - x) * (g_x - x) + (g_y - y) * (g_y - y));

        if ((int)distance == 0) return;

        double cos = (g_x - x) / distance;
        double sin = (g_y - y) / distance;

        this->angle = acos(cos);

        if (v > distance) v = distance;

        x = (int)(v * cos + x);
        y = (int)(v * sin + y);

        this->set_Location(Point(x, y));
        this->Refresh();
    }

    virtual void OnPaint()
    {
        Form::OnPaint();
        _P<Graphics> g = this->CreateGraphics();
        g->FillEllipse(Color::get_Blue(), 0, 0, 14, 14);
        g->FillEllipse(Color::get_Red(), 7 * cos(angle), 7 * sin(angle), 3, 3);
        g->Dispose();
    }


public:
    static void Main(_A<String> args)
    {
        Application::Run(new Form1());
    }
};

SET_MAIN_CLASS(Form1)
