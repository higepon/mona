#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmessage.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Terminal : public Control
{
public:
	Terminal() : x(0), y(0)
	{
		this->set_BackColor(Color::get_White());
	}
	
	virtual ~Terminal() {}

	void Output(String s)
	{
		_P<Graphics> g = this->CreateGraphics();
		String ss;
		int w = this->get_Width(), h = this->get_Height();
		wchar prev = 0;
		FOREACH (wchar, ch, s)
		{
			if (ch == '\r' || ch == '\n')
			{
				if (prev == '\r' && ch == '\n') continue;
				if (ss.get_Length() > 0)
				{
					g->DrawString(ss, Control::get_DefaultFont(), Color::get_Black(), x, y);
					ss.Unset();
				}
				x = 0;
				if (y < h - 24)
				{
					y += 12;
				}
				else
				{
					Color* img = this->buffer->get();
					int len = (h - 12) * w, d = w * 12;
					for (int i = 0; i < len; i++)
					{
						img[i] = img[i + d];
					}
					x = 0;
					g->FillRectangle(this->get_BackColor(), 0, y, w, h - y);
				}
			}
			else
			{
				ss += ch;
			}
			prev = ch;
		}
		if (ss.get_Length() > 0)
		{
			Size sz = g->MeasureString(ss, Control::get_DefaultFont());
			g->DrawString(ss, Control::get_DefaultFont(), Color::get_Black(), x, y);
			x += sz.Width;
		}
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
	MessageInfo msg;
	monapi_cmessage_receive_header_only(&msg, MSG_SERVER_START_OK);
	stdout_tid = msg.from;
	MonAPI::Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
}

class Form1 : public Form
{
public:
	Form1()
	{
		this->InitializeComponent();
		
		terminal = new Terminal();
		terminal->set_Bounds(Rectangle(0, 0, 400, 256));
		this->get_Controls()->Add(terminal.get());
	}
	
private:
	void InitializeComponent()
	{
		this->set_Location(Point(128, 128));
		this->set_ClientSize(Size(400, 256));
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
