/*
Copyright (c) 2004 Tino, bayside
All rights reserved.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <baygui.h>

extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

CommonParameters* __commonParams;
dword __gui_server;
static dword commonParamsHandle;

namespace baygui
{
	_P<ArrayList<_P<Window> > > Application::forms;
	_P<Control> Application::prevControl;
	_P<Window> Application::mainWindow;
	bool Application::isExited = false;
	int Application::mouseButtons = 0;
	unsigned char* Application::defaultFontData = NULL;
	
	void Application::initialize()
	{
		if (isInDLL(__CTOR_LIST__)) invokeFuncList(__CTOR_LIST__);
		Application::forms = new ArrayList<_P<Window> >;
		
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
	}
	
	void Application::dispose()
	{
		monapi_register_to_server(ID_GUI_SERVER, MONAPI_FALSE);
		MonAPI::MemoryMap::unmap(commonParamsHandle);
		if (isInDLL(__CTOR_LIST__)) invokeFuncList(__DTOR_LIST__);
	}
	
	void Application::run()
	{
		MessageInfo msg;
		while (!Application::isExited)
		{
			if (MonAPI::Message::receive(&msg) != 0) continue;
			Application::processEvent(&msg);
		}
		Application::prevControl = NULL;
	}
	
	void Application::processEvent(MessageInfo* m)
	{
		switch (m->header)
		{
			case MSG_MOUSE_INFO:
			{
				_P<Control> c;
				if (Application::mouseButtons != 0) {
					c = Application::prevControl;
				} else {
					_P<Window> f = Application::findWindow(m->arg1, m->arg2);
					if (f != NULL) c = f->findChild(m->arg1, m->arg2);
				}
				
				Point p;
				MouseEvent me;
				if (c != NULL) {
					p = c->pointToClient(Point(m->arg1, m->arg2));
					me.button = m->arg3;
					me.x = p.X;
					me.y = p.Y;
					me.source = c.get();
				}
				if (Application::mouseButtons != (int)m->arg3) {
					if (m->arg3 == 0) {
						if (c != NULL)
						{
							me.button = Application::mouseButtons;
							me.type = MOUSE_RELEASED;
							c->postEvent(&me);
						}
					} else {
						if (c != NULL) {
							me.type = MOUSE_PRESSED;
							c->postEvent(&me);
						}
						Application::prevControl = c;
					}
					Application::mouseButtons = m->arg3;
				} else if (c != NULL) {
					me.type = MOUSE_MOVED;
					c->postEvent(&me);
				}
				
				break;
			}
			case MSG_KEY_VIRTUAL_CODE:
			{
				KeyEvent ke;
				
				if (Application::mainWindow != NULL) {
					ke.keycode = m->arg1;
					ke.modifiers = m->arg2;
					if (m->arg2 & KEY_MODIFIER_DOWN)
					{
						ke.type = KEY_PRESSED;
					}
					else if (m->arg2 & KEY_MODIFIER_UP)
					{
						ke.type = KEY_RELEASED;
					}
					mainWindow->postEvent(&ke);
				}
				break;
			}
			case TIMER:
			{
				Event te;
				
				if (Application::mainWindow != NULL)
				{
					te.type = TIMER;
					te.source = mainWindow.get();
					mainWindow->postEvent(&te);
				}
				break;
			}
		}
	}
	
	void Application::run(_P<Window> form)
	{
		form->setVisible(true);
		Application::mainWindow = form;
		Application::run();
	}
	
	void Application::doEvents()
	{
		if (Application::isExited) return;
		
		MessageInfo msg;
		while (MonAPI::Message::exist()) {
			if (!MonAPI::Message::receive(&msg)) {
				Application::processEvent(&msg);
			}
		}
	}
	
	void Application::exit()
	{
		Application::isExited = true;
	}
	
	void Application::addWindow(_P<Window> f)
	{
		Application::forms->Add(f);
	}
	
	void Application::removeWindow(_P<Window> f)
	{
		Application::forms->Remove(f);
		if (Application::mainWindow == f) {
			Application::mainWindow = NULL;
			Application::exit();
		}
	}
	
	_P<Window> Application::findWindow(int x, int y)
	{
		FOREACH_AL(_P<Window>, f, Application::forms)
		{
			if (f->getVisible() && f->getRect().Contains(x, y)) return f;
		}
		END_FOREACH_AL
		return NULL;
	}
}
