// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

#if !defined(MONA) && defined(WIN32)
extern void MonaGUI_Initialize();
extern void MonaGUI_Run();
extern void MonaGUI_Exit();
#endif

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#ifdef MONA
enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_RETURNFONT
};

static int SendMessage(dword to, dword header, dword arg1, dword arg2, dword arg3)
{
	MessageInfo msg;
	MonAPI::Message::create(&msg, header, arg1, arg2, arg3, NULL);
	return MonAPI::Message::send(to, &msg);
}

static MessageInfo WaitMessage(dword header)
{
	MessageInfo msg;
	for (;;)
	{
		if (MonAPI::Message::receive(&msg)) continue;
		if (msg.header == header) break;
	}
	return msg;
}
#else
#include "MONA-12.h"
#endif

namespace System { namespace Mona { namespace Forms
{
	_P<ArrayList<_P<Form> > > Application::forms;
	_P<Control> Application::prevControl;
	_P<Form> Application::mainForm;
	bool Application::isExited = false;
	int Application::mouseButtons = 0;
	unsigned char* Application::defaultFontData = NULL;
	
	void Application::Initialize()
	{
		Application::forms = new ArrayList<_P<Form> >;
		
#ifdef MONA
		dword msvr = MonAPI::Message::lookupMainThread("MOUSE.SVR");
		if (msvr == 0xFFFFFFFF ||
			::SendMessage(msvr, MSG_MOUSE_REGIST_TO_SERVER, MonAPI::System::getThreadID(), 0, 0) != 0)
		{
			::printf("ERROR: Can't connect to mouse server!\n");
			::exit(1);
		}
		
		dword gsvr = MonAPI::Message::lookupMainThread("GUI.SVR");
		if (gsvr == 0xFFFFFFFF || ::SendMessage(gsvr, MSG_GUISERVER_GETFONT, 0, 0, 0) != 0)
		{
			::printf("ERROR: Can't connect to GUI server!\n");
			::exit(1);
		}
		MessageInfo msg = WaitMessage(MSG_GUISERVER_RETURNFONT);
		byte* font_data = MonAPI::MemoryMap::map(msg.arg1);
		if (font_data == NULL)
		{
			::printf("ERROR: Can not get font data!\n");
			::exit(1);
		}
		Application::defaultFontData = new unsigned char[msg.arg2];
		::memcpy(Application::defaultFontData, font_data, msg.arg2);
		MonAPI::MemoryMap::unmap(msg.arg1);
#else
		Application::defaultFontData = MONA_12_MNF;
#ifdef WIN32
		::MonaGUI_Initialize();
#endif
#endif
	}
	
	void Application::Dispose()
	{
#ifdef MONA
		dword msvr = MonAPI::Message::lookupMainThread("MOUSE.SVR");
		if (msvr == 0xFFFFFFFF ||
			::SendMessage(msvr, MSG_MOUSE_UNREGIST_FROM_SERVER, MonAPI::System::getThreadID(), 0, 0) != 0)
		{
			::printf("ERROR: Can't connect to mouse server!\n");
			::exit(1);
		}
#endif
	}
	
	void Application::Run()
	{
#if defined(MONA)
		MessageInfo msg;
		while (!Application::isExited)
		{
			if (!MonAPI::Message::receive(&msg))
			{
				Application::ProcessEvent(msg.header, msg.arg1, msg.arg2, msg.arg3);
			}
		}
#elif defined(WIN32)
		::MonaGUI_Run();
#endif
	}
	
	void Application::ProcessEvent(unsigned int message, unsigned int arg1, unsigned int arg2, unsigned int arg3)
	{
		switch (message)
		{
			case 0x0a /*MSG_MOUSE_INFO*/:
			{
				_P<Control> c;
				if (Application::mouseButtons != 0)
				{
					c = Application::prevControl;
				}
				else
				{
					_P<Form> f = Application::FindForm(arg1, arg2);
					if (f != NULL) c = f->FindControl(arg1, arg2);
				}
				
				Point p;
				_P<MouseEventArgs> e;
				if (c != NULL)
				{
					p = c->PointToClient(Point(arg1, arg2));
					e = new MouseEventArgs(arg3, p.X, p.Y);
				}
				if (Application::mouseButtons != (int)arg3)
				{
					if (arg3 == 0)
					{
						if (c != NULL)
						{
							e->Button = Application::mouseButtons;
							c->WndProc(WM_MOUSEUP, e.get());
						}
					}
					else
					{
						if (c != NULL) c->WndProc(WM_MOUSEDOWN, e.get());
						Application::prevControl = c;
					}
					Application::mouseButtons = arg3;
				}
				else if (c != NULL)
				{
					c->WndProc(WM_MOUSEMOVE, e.get());
				}
				
				break;
			}
		}
	}
	
	void Application::Run(_P<Form> form)
	{
		form->Show();
		Application::mainForm = form;
		Application::Run();
	}
	
	void Application::Exit()
	{
		Application::isExited = true;
#if !defined(MONA) && defined(WIN32)
		::MonaGUI_Exit();
#endif
	}
	
	void Application::AddForm(_P<Form> f)
	{
		Application::forms->Add(f);
	}
	
	void Application::RemoveForm(_P<Form> f)
	{
		Application::forms->Remove(f);
		if (Application::mainForm == f) Application::Exit();
	}
	
	_P<Form> Application::FindForm(int x, int y)
	{
		FOREACH_AL(_P<Form>, f, Application::forms)
		{
			if (f->CheckPoint(x, y)) return f;
		}
		return NULL;
	}
}}}
