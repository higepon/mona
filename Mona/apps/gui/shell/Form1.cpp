#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Terminal : public Control
{
public:
	Terminal() : x(0), y(0) {}
	virtual ~Terminal() {}

	void Output(String s)
	{
		_P<Graphics> g = this->CreateGraphics();
		Size sz = g->MeasureString(s, Control::get_DefaultFont());
		g->DrawString(s, Control::get_DefaultFont(), Color::get_Black(), x, y);
		y += sz.Height;
		g->Dispose();
	}

private:
	int x, y;
};

static dword my_tid, stdout_tid;
static _P<Terminal> terminal;

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
				terminal->Output(msg.str);
				terminal->Refresh();
				MonAPI::Message::reply(&msg);
				break;
			}
		}
	}
}

static void InitThread()
{
	my_tid = syscall_get_tid();
	dword id = syscall_mthread_create((dword)StdoutMessageLoop);
	syscall_mthread_join(id);
	for (MessageInfo msg;;)
	{
		if (MonAPI::Message::receive(&msg) != 0) continue;
		
		if (msg.header == MSG_SERVER_START_OK)
		{
			stdout_tid = msg.from;
			break;
		}
	}
	MonAPI::Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
}

class Form1 : public Form
{
public:
	Form1()
	{
		this->InitializeComponent();
		
		terminal = new Terminal();
		terminal->set_Bounds(Rectangle(0, 0, 256, 160));
		this->get_Controls()->Add(terminal.get());
	}
	
private:
	void InitializeComponent()
	{
		this->set_Location(Point(128, 128));
		this->set_ClientSize(Size(256, 160));
		this->set_Text("GUI Shell");
	}
	
public:
	static void Main(_A<String> args)
	{
		_P<Form> f = new Form1();
		InitThread();
		Application::Run(f);
		MonAPI::Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
		syscall_kill_thread(stdout_tid);
	}
};

SET_MAIN_CLASS(Form1)
