// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>

#define MSG_GUI_TIMER 0x40f0

#ifdef MONA
#include <monapi/messages.h>
#elif defined(WIN32)
extern void MonaGUI_Initialize();
extern void MonaGUI_Run();
extern void MonaGUI_Exit();
#endif

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#ifdef MONA
dword __gui_server;
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
		if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) ::exit(1);
		if (!monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_TRUE)) ::exit(1);
		__gui_server = monapi_get_server_thread_id(ID_GUI_SERVER);
		if (__gui_server == THREAD_UNKNOWN) ::exit(1);
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, __gui_server, MSG_GUISERVER_GETFONT) != 0)
		{
			::printf("%s:%d:ERROR: can not connect to GUI server!\n", __FILE__, __LINE__);
			::exit(1);
		}
		byte* font_data = MonAPI::MemoryMap::map(msg.arg2);
		if (font_data == NULL)
		{
			::printf("%s:%d:ERROR: Can not get font data!\n", __FILE__, __LINE__);
			::exit(1);
		}
		Application::defaultFontData = new unsigned char[msg.arg3];
		::memcpy(Application::defaultFontData, font_data, msg.arg3);
		MonAPI::MemoryMap::unmap(msg.arg2);
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
		monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
		monapi_register_to_server(ID_MOUSE_SERVER, MONAPI_FALSE);
#endif
	}
	
	void Application::Run()
	{
#if defined(MONA)
		MessageInfo msg;
		while (!Application::isExited)
		{
			if (MonAPI::Message::receive(&msg) != 0) continue;
			
			Application::ProcessEvent(msg.header, msg.arg1, msg.arg2, msg.arg3);
		}
#elif defined(WIN32)
		::MonaGUI_Run();
#endif
		Application::prevControl = NULL;
	}
	
	void Application::ProcessEvent(unsigned int message, unsigned int arg1, unsigned int arg2, unsigned int arg3)
	{
		switch (message)
		{
			case MSG_MOUSE_INFO:
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
			case MSG_GUI_TIMER:
				((Timer*)arg1)->OnTick(EventArgs::get_Empty());
				break;
		}
	}
	
	void Application::Run(_P<Form> form)
	{
		form->Show();
		Application::mainForm = form;
		Application::Run();
	}
	
	void Application::DoEvents()
	{
		if (Application::isExited) return;
		
#if defined(MONA)
		MessageInfo msg;
		while (MonAPI::Message::exist())
		{
			if (!MonAPI::Message::receive(&msg))
			{
				Application::ProcessEvent(msg.header, msg.arg1, msg.arg2, msg.arg3);
			}
		}
#endif
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
		if (Application::mainForm == f)
		{
			Application::mainForm = NULL;
			Application::Exit();
		}
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
