// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/Cursor.h>

#ifdef MONA
#include <monapi/messages.h>
#include <servers/gui.h>
#include <map>
extern std::map<dword, System::Mona::Forms::Control*> mapControls;
extern std::map<dword, System::Mona::Forms::Timer*> mapTimers;
#elif defined(WIN32)
extern void MonaGUI_Initialize();
extern void MonaGUI_Run();
extern void MonaGUI_Exit();
#endif

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#ifdef MONA
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

CommonParameters* __commonParams;
dword __gui_server;
static dword commonParamsHandle;
#else
#include "MONA-12.h"
#endif

namespace System { namespace Mona { namespace Forms
{
	_P<ArrayList<_P<Form> > > Application::forms;
	_P<Form> Application::mainForm;
	_P<ArrayList<IMessageFilter*> > Application::messageFilters;
	bool Application::isExited = false;
	int Application::mouseButtons = 0;
	unsigned char* Application::defaultFontData = NULL;
	
	void Application::Initialize()
	{
#ifdef MONA
		if (isInDLL(__CTOR_LIST__)) invokeFuncList(__CTOR_LIST__);
#endif
		Application::forms = new ArrayList<_P<Form> >;
		Application::messageFilters = new ArrayList<IMessageFilter*>;
		
#ifdef MONA
		MessageInfo msg;
		if (MonAPI::Message::sendReceive(&msg, monapi_get_server_thread_id(ID_PROCESS_SERVER), MSG_PROCESS_GET_COMMON_PARAMS) != 0)
		{
			printf("MouseServer: can not get common parameters\n");
			::exit(1);
		}
		commonParamsHandle = msg.arg2;
		__commonParams = (CommonParameters*)MonAPI::MemoryMap::map(commonParamsHandle);
		
		if (!monapi_register_to_server(ID_GUI_SERVER, MONAPI_TRUE)) ::exit(1);
		__gui_server = monapi_get_server_thread_id(ID_GUI_SERVER);
		if (__gui_server == THREAD_UNKNOWN) ::exit(1);
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
		MonAPI::MemoryMap::unmap(commonParamsHandle);
		if (isInDLL(__CTOR_LIST__)) invokeFuncList(__DTOR_LIST__);
#endif
	}
	
	void Application::Run()
	{
#if defined(MONA)
		MessageInfo msg;
		while (!Application::isExited)
		{
			if (MonAPI::Message::receive(&msg) != 0) continue;
			
			Message m = { msg.from, msg.header, msg.arg1, msg.arg2, msg.arg3 };
			Application::ProcessEvent(&m);
		}
#elif defined(WIN32)
		::MonaGUI_Run();
#endif
	}
	
	void Application::ProcessEvent(Message* m)
	{
		FOREACH_AL (IMessageFilter*, mf, Application::messageFilters)
		{
			if (mf->PreFilterMessage(m)) return;
		}
		END_FOREACH_AL
		
		Point mp = Cursor::get_Position();
		switch (m->header)
		{
			case MSG_GUISERVER_MOUSEMOVE:
			case MSG_GUISERVER_MOUSEDOWN:
			case MSG_GUISERVER_MOUSEUP:
			{
				_P<Control> c = mapControls[m->arg1];
				if (c == NULL) break;
				
				Point p1 = Point(GET_X_DWORD(m->arg2), GET_Y_DWORD(m->arg2));
				Point p2 = c->PointToClient(p1);
				if (m->header == MSG_GUISERVER_MOUSEMOVE)
				{
					if (p1 != Cursor::get_Position()) break;
				}
				else if (mp != p1)
				{
					_P<MouseEventArgs> e1 = new MouseEventArgs(m->arg3, p2.X, p2.Y);
					c->WndProc(MSG_GUISERVER_MOUSEMOVE, e1.get());
				}
				_P<MouseEventArgs> e2 = new MouseEventArgs(m->arg3, p2.X, p2.Y);
				c->WndProc(m->header, e2.get());
				mp = p1;
				break;
			}
			case MSG_GUISERVER_ACTIVATED:
			case MSG_GUISERVER_DEACTIVATE:
			case MSG_GUISERVER_MOUSEENTER:
			case MSG_GUISERVER_MOUSELEAVE:
			{
				_P<Control> c = mapControls[m->arg1];
				if (c != NULL) c->WndProc(m->header, EventArgs::get_Empty());
				break;
			}
			case MSG_GUISERVER_KEYDOWN:
			case MSG_GUISERVER_KEYUP:
			{
				_P<Control> c = mapControls[m->arg1];
				if (c != NULL)
				{
					_P<KeyEventArgs> e = new KeyEventArgs(m->arg2, m->arg3);
					c->WndProc(m->header, e.get());
				}
				break;
			}
			case MSG_GUISERVER_DISPOSEWINDOW:
			{
				Application::Exit();
				MessageInfo info;
				info.from = m->from;
				info.header = m->header;
				info.arg1 = m->arg1;
				info.arg2 = m->arg2;
				info.arg3 = m->arg3;
				MonAPI::Message::reply(&info);
				break;
			}
			case MSG_TIMER:
				if (mapTimers.find(m->arg1) != mapTimers.end())
				{
					mapTimers[m->arg1]->OnTick(EventArgs::get_Empty());
				}
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
				Message m = { msg.from, msg.header, msg.arg1, msg.arg2, msg.arg3 };
				Application::ProcessEvent(&m);
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
	
	void Application::AddMessageFilter(IMessageFilter* value)
	{
		Application::messageFilters->Add(value);
	}
	
	void Application::RemoveMessageFilter(IMessageFilter* value)
	{
		Application::messageFilters->Remove(value);
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
}}}
