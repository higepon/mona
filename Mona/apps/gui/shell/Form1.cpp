#include <monapi/syscall.h>
#include <monapi/messages.h>
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
	
	void Output(String s)
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
				msg.str[127] = '\0';
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
	MessageInfo msg, src;
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	stdout_tid = msg.from;
}

class Form1 : public Form
{
public:
	Form1()
	{
		this->InitializeComponent();
		
		terminal = new Terminal();
		terminal->set_Bounds(Rectangle(Point::get_Empty(), this->get_ClientSize()));
		this->get_Controls()->Add(terminal.get());
	}
	
private:
	void InitializeComponent()
	{
		this->set_Location(Point(64, 64));
		this->set_ClientSize(Size(SHELL_WIDTH * FONT_WIDTH + 4, SHELL_HEIGHT * FONT_HEIGHT + 4));
		this->set_Text("GUI Shell");
		this->set_Opacity(0.8);
	}
	
public:
	static void Main(_A<String> args)
	{
		_P<Form> f = new Form1();
		InitThread();
		MonAPI::Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_GRAB_STDOUT, stdout_tid);
		Application::Run(f);
		MonAPI::Message::sendReceive(NULL, PROCESS_STDOUT_THREAD, MSG_PROCESS_UNGRAB_STDOUT, stdout_tid);
		syscall_kill_thread(stdout_tid);
	}
};

SET_MAIN_CLASS(Form1)
