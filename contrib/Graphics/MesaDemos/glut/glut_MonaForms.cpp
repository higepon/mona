// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Cursor.h>
#include <gui/System/Mona/Forms/Form.h>
#include <GL/glut.h>
#include <GL/osmesa.h>
#include "glutint.h"

#define WARN_NOT_IMPLEMENTED(FORMAT, ARG) //printf("GLUT: not implemented: "FORMAT"\n", ARG)

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Mona::Forms;

char *__glutProgramName = NULL;

static Point pos;
static Size size(256, 256);
static uint32_t start_tick = 0;
static int modifiers = 0;
static bool exited = false, doubleBuffered = false, numLock = false;

class _glut_window : public Form
{
private:
	OSMesaContext context;
	int buttons, id, buf, bufsum;
	bool doubleBuffered, redisplay, reshaped;
	_A<_A<Color> > glbuf;
	
public:
	static _P<ArrayList<_P<_glut_window> > > list;
	
	void (GLUTCALLBACK *idle)(void);
	void (GLUTCALLBACK *display)(void);
	void (GLUTCALLBACK *reshape)(int width, int height);
	void (GLUTCALLBACK *keyboard)(unsigned char key, int x, int y);
	void (GLUTCALLBACK *mouse)(int button, int state, int x, int y);
	void (GLUTCALLBACK *motion)(int x, int y);
	void (GLUTCALLBACK *entry)(int state);
	void (GLUTCALLBACK *passive_motion)(int x, int y);
	void (GLUTCALLBACK *visibility)(int state);
	void (GLUTCALLBACK *special)(int key, int x, int y);
	
	_glut_window() : context(NULL), buttons(0), id(0), buf(0), bufsum(0),
		doubleBuffered(false), redisplay(true), reshaped(true),
		idle(NULL), display(NULL), reshape(NULL), keyboard(NULL),
		mouse(NULL), motion(NULL), entry(NULL),
		passive_motion(NULL), visibility(NULL), special(NULL)
	{
		//this->set_BackColor(Color::get_Black());
		this->set_Location(::pos);
		this->set_ClientSize(::size);
	}
	
	virtual ~_glut_window()
	{
	}
	
	virtual void Create()
	{
		this->doubleBuffered = ::doubleBuffered;
		this->InitBuffers();
		list->Add(this);
		this->id = list->get_Count();
		this->context = OSMesaCreateContext(OSMESA_BGRA, NULL);
		glutSetWindow(this->id);
		
		Form::Create();
	}
	
	virtual void Dispose()
	{
		glutSetWindow(0);
		Form::Dispose();
		//list->Remove(this);
		if (this->context != NULL)
		{
			OSMesaDestroyContext(this->context);
			this->context = NULL;
		}
		exited = true;
	}
	
	virtual void Show()
	{
		glutSetWindow(this->get_ID());
		if (this->visibility != NULL) (*this->visibility)(GLUT_VISIBLE);
		this->Reshape();
		this->Redisplay();
		Form::Show();
	}
	
	void Reshape(int width, int height)
	{
		Size sz = Size(width, height), sz2 = this->get_ClientSize();
		if (width != sz2.Width || height != sz2.Height)
		{
			bool v = this->get_Visible();
			Form::Dispose();
			this->set_ClientSize(sz);
			this->InitBuffers();
			Form::Create();
			if (v) this->Show();
		}
		this->reshaped = true;
	}
	
	bool Reshape()
	{
		if (!this->reshaped) return false;
		
		if (this->reshape != NULL)
		{
			glutSetWindow(this->id);
			Size sz = this->get_ClientSize();
			(*this->reshape)(sz.Width, sz.Height);
		}
		this->reshaped = false;
		return true;
	}
	
	int get_ID()
	{
		return this->id;
	}
	
	bool MakeCurrent()
	{
		Size sz = this->get_ClientSize();
		return OSMesaMakeCurrent(
			this->context, this->glbuf[this->buf].get(),
			GL_UNSIGNED_BYTE, sz.Width, sz.Height) == GL_TRUE;
	}
	
	void SwapBuffers()
	{
		if (!this->doubleBuffered) return;
		
		this->buf = 1 - this->buf;
		this->MakeCurrent();
	}
	
	void PostRedisplay()
	{
		this->redisplay = true;
	}
	
	bool Redisplay()
	{
		int sum = 0;
		if (!this->redisplay)
		{
			if (!this->doubleBuffered)
			{
				sum = this->GetBufferSum();
				if (sum == this->bufsum) return false;
			}
			else
			{
				return false;
			}
		}
		
		if (this->display != NULL)
		{
			glutSetWindow(this->id);
			(*this->display)();
			if (!this->doubleBuffered) sum = this->GetBufferSum();
		}
		this->Refresh();
		this->redisplay = false;
		this->bufsum = sum;
		return true;
	}
	
protected:
	void InitBuffers()
	{
		Size sz = this->get_ClientSize();
		int len = sz.Width * sz.Height;
		if (this->doubleBuffered)
		{
			this->glbuf.Alloc(2);
			this->glbuf[0].Alloc(len);
			this->glbuf[1].Alloc(len);
		}
		else
		{
			this->glbuf.Alloc(1);
			this->glbuf[0].Alloc(len);
		}
	}
	
	int GetBufferSum()
	{
		Size sz = this->get_ClientSize();
		int ret = 0, len = sz.Width * sz.Height;
		int* p = (int*)this->GetBuffer();
		for (int i = 0; i < len; i++, p++)
		{
			asm volatile("roll $1, %0" :: "m"(ret));
			ret ^= *p;
		}
		return ret;
	}
	
	inline Color* GetBuffer()
	{
		return this->doubleBuffered ? this->glbuf[1 - this->buf].get() : this->glbuf[0].get();
	}
	
	virtual void OnPaint()
	{
		Form::OnPaint();
		
		Size sz = this->get_ClientSize();
		int w = this->get_Width(), dx = w - sz.Width;
		unsigned int* dst = (unsigned int*)(this->buffer->get() + this->offset.X + this->offset.Y * w);
		unsigned int* src = (unsigned int*)(this->GetBuffer() + (sz.Height - 1) * sz.Width);
		for (int y = 0; y < sz.Height; y++)
		{
			for (int x = 0; x < sz.Width; x++, dst++, src++)
			{
				*dst = *src | 0xff000000;
			}
			dst += dx;
			src -= sz.Width * 2;
		}
	}
	
	virtual void OnKeyDown(_P<KeyEventArgs> e)
	{
		::modifiers = e->Modifiers;
		Point p = this->PointToClient(Cursor::get_Position());
		int key = 0, sp = 0;
		switch (e->KeyCode)
		{
			case MonAPI::Keys::F1:  sp = GLUT_KEY_F1;  break;
			case MonAPI::Keys::F2:  sp = GLUT_KEY_F2;  break;
			case MonAPI::Keys::F3:  sp = GLUT_KEY_F3;  break;
			case MonAPI::Keys::F4:  sp = GLUT_KEY_F4;  break;
			case MonAPI::Keys::F5:  sp = GLUT_KEY_F5;  break;
			case MonAPI::Keys::F6:  sp = GLUT_KEY_F6;  break;
			case MonAPI::Keys::F7:  sp = GLUT_KEY_F7;  break;
			case MonAPI::Keys::F8:  sp = GLUT_KEY_F8;  break;
			case MonAPI::Keys::F9:  sp = GLUT_KEY_F9;  break;
			case MonAPI::Keys::F10: sp = GLUT_KEY_F10; break;
			case MonAPI::Keys::F11: sp = GLUT_KEY_F11; break;
			case MonAPI::Keys::F12: sp = GLUT_KEY_F12; break;
			
			case MonAPI::Keys::Left:     sp = GLUT_KEY_LEFT;      break;
			case MonAPI::Keys::Up:       sp = GLUT_KEY_UP;        break;
			case MonAPI::Keys::Right:    sp = GLUT_KEY_RIGHT;     break;
			case MonAPI::Keys::Down:     sp = GLUT_KEY_DOWN;      break;
			case MonAPI::Keys::PageUp:   sp = GLUT_KEY_PAGE_UP;   break;
			case MonAPI::Keys::PageDown: sp = GLUT_KEY_PAGE_DOWN; break;
			case MonAPI::Keys::Home:     sp = GLUT_KEY_HOME;      break;
			case MonAPI::Keys::End:      sp = GLUT_KEY_END;       break;
			case MonAPI::Keys::Insert:   sp = GLUT_KEY_INSERT;    break;
			
			case MonAPI::Keys::NumLock:
				numLock = !numLock;
				break;
			
			case MonAPI::Keys::NumPad0:
				if (numLock) key = '0'; else sp = GLUT_KEY_INSERT;
				break;
			case MonAPI::Keys::NumPad1:
				if (numLock) key = '1'; else sp = GLUT_KEY_END;
				break;
			case MonAPI::Keys::NumPad2:
				if (numLock) key = '2'; else sp = GLUT_KEY_DOWN;
				break;
			case MonAPI::Keys::NumPad3:
				if (numLock) key = '3'; else sp = GLUT_KEY_PAGE_DOWN;
				break;
			case MonAPI::Keys::NumPad4:
				if (numLock) key = '4'; else sp = GLUT_KEY_LEFT;
				break;
			case MonAPI::Keys::NumPad6:
				if (numLock) key = '6'; else sp = GLUT_KEY_RIGHT;
				break;
			case MonAPI::Keys::NumPad7:
				if (numLock) key = '7'; else sp = GLUT_KEY_HOME;
				break;
			case MonAPI::Keys::NumPad8:
				if (numLock) key = '8'; else sp = GLUT_KEY_UP;
				break;
			case MonAPI::Keys::NumPad9:
				if (numLock) key = '9'; else sp = GLUT_KEY_PAGE_UP;
				break;
			case MonAPI::Keys::Decimal:
				if (numLock) key = '.'; else key = 21; // Delete
				break;
			
			case MonAPI::Keys::Delete:
				key = 21;
				break;
			case MonAPI::Keys::Separator:
				key = 13; // Enter
				break;
			
			default:
				if (e->KeyCode < ' ')
				{
					key = e->KeyCode;
				}
				else
				{
					KeyInfo ki = { e->KeyCode, e->Modifiers };
					key = MonAPI::Keys::ToChar(ki);
				}
				break;
		}
		if (sp != 0)
		{
			if (this->special != NULL)
			{
				glutSetWindow(this->id);
				(*this->special)(sp, p.X, p.Y);
			}
		}
		else if (key != 0)
		{
			if (this->keyboard != NULL)
			{
				glutSetWindow(this->id);
				(*this->keyboard)(key, p.X, p.Y);
			}
		}
		
		Form::OnKeyDown(e);
	}
	
	virtual void OnMouseDown(_P<MouseEventArgs> e)
	{
		if (this->mouse != NULL)
		{
			glutSetWindow(this->id);
			int button = e->Button == 1 ? GLUT_LEFT_BUTTON : GLUT_RIGHT_BUTTON;
			(*this->mouse)(button, GLUT_DOWN, e->X, e->Y);
			if (this->buttons == 0) this->set_Capture(true);
			this->buttons |= e->Button;
		}
		Form::OnMouseDown(e);
	}
	
	virtual void OnMouseUp(_P<MouseEventArgs> e)
	{
		if (this->mouse != NULL)
		{
			glutSetWindow(this->id);
			int button = e->Button == 1 ? GLUT_LEFT_BUTTON : GLUT_RIGHT_BUTTON;
			(*this->mouse)(button, GLUT_UP, e->X, e->Y);
			this->buttons &= ~e->Button;
			if (this->buttons == 0) this->set_Capture(false);
		}
		Form::OnMouseUp(e);
	}
	
	virtual void OnMouseMove(_P<MouseEventArgs> e)
	{
		if (this->buttons == 0)
		{
			if (this->passive_motion != NULL)
			{
				glutSetWindow(this->id);
				(*this->passive_motion)(e->X, e->Y);
			}
		}
		else
		{
			if (this->motion != NULL)
			{
				glutSetWindow(this->id);
				(*this->motion)(e->X, e->Y);
			}
		}
		
		Form::OnMouseMove(e);
	}
	
	virtual void OnMouseEnter(_P<EventArgs> e)
	{
		if (this->entry != NULL)
		{
			glutSetWindow(this->id);
			(*this->entry)(GLUT_ENTERED);
		}
		
		Form::OnMouseEnter(e);
	}
	
	virtual void OnMouseLeave(_P<EventArgs> e)
	{
		if (this->entry != NULL)
		{
			glutSetWindow(this->id);
			(*this->entry)(GLUT_LEFT);
		}
		
		Form::OnMouseLeave(e);
	}
};

static _P<_glut_window> current = NULL;

#define windows _glut_window::list
_P<ArrayList<_P<_glut_window> > > windows;

#ifdef exit
#undef exit
#endif

extern "C" void _glut_exit(int code)
{
    _printf("[1]");
	if (windows->get_Count() > 0)
	{
    _printf("[2]");
		exited = true;
	}
	else
	{
    _printf("[3]");
		exit(code);
    _printf("[4]");
	}
    _printf("[5]");
}

extern "C" void _mona_set_exit(void(*_exit)(int code));
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

#undef exit
static void(*_p_exit)(int code) = NULL;

void _mona_set_exit(void(*_exit)(int code))
{
	_p_exit = _exit;
}

void _mona_exit(int code)
{
	if (_p_exit == NULL)
	{
		exit(code);
	}
	else
	{
		(*_p_exit)(code);
	}
}

GLUTAPI void GLUTAPIENTRY glutInit(int *argcp, char **argv)
{
	if (isInDLL(__CTOR_LIST__)) invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
	Application::Initialize();
	windows = new ArrayList<_P<_glut_window> >;
	__glutProgramName = argv[0];
	_mona_set_exit(_glut_exit);
	start_tick = syscall_get_tick();
}

GLUTAPI void GLUTAPIENTRY glutInitDisplayMode(unsigned int mode)
{
	doubleBuffered = (mode & GLUT_DOUBLE) != 0;
}

GLUTAPI void GLUTAPIENTRY glutInitWindowPosition(int x, int y)
{
	pos = Point(x, y);
}

GLUTAPI void GLUTAPIENTRY glutInitWindowSize(int width, int height)
{
	size = Size(width, height);
}

GLUTAPI int GLUTAPIENTRY glutCreateWindow(const char *title)
{
	_P<_glut_window> w = new _glut_window();
	w->set_Text(title);
	w->Create();
	return w->get_ID();
}

GLUTAPI void GLUTAPIENTRY glutDestroyWindow(int win)
{
	if (win < 1) return;
	
	windows->get_Item(win - 1)->Dispose();
}

GLUTAPI void GLUTAPIENTRY glutShowWindow(void)
{
	if (current != NULL) current->Show();
}

GLUTAPI void GLUTAPIENTRY glutHideWindow(void)
{
	if (current != NULL) current->Hide();
}

GLUTAPI void GLUTAPIENTRY glutSwapBuffers(void)
{
	if (current != NULL) current->SwapBuffers();
}

GLUTAPI int GLUTAPIENTRY glutGetWindow(void)
{
	return current == NULL ? 0 : current->get_ID();
}

GLUTAPI void GLUTAPIENTRY glutSetWindow(int win)
{
	if (current != NULL && current->get_ID() == win) return;
	if (win < 1)
	{
		current = NULL;
		return;
	}
	
	current = windows->get_Item(win - 1);
	current->MakeCurrent();
}

GLUTAPI void GLUTAPIENTRY glutSetWindowTitle(const char *title)
{
	if (current != NULL) current->set_Text(title);
}

GLUTAPI void GLUTAPIENTRY glutPositionWindow(int x, int y)
{
	if (current != NULL) current->set_Location(Point(x, y));
}

GLUTAPI void GLUTAPIENTRY glutDisplayFunc(void (GLUTCALLBACK *func)(void))
{
	if (current != NULL) current->display = func;
}

GLUTAPI void GLUTAPIENTRY glutReshapeFunc(void (GLUTCALLBACK *func)(int width, int height))
{
	if (current != NULL) current->reshape = func;
}

GLUTAPI void GLUTAPIENTRY glutKeyboardFunc(void (GLUTCALLBACK *func)(unsigned char key, int x, int y))
{
	if (current != NULL) current->keyboard = func;
}

GLUTAPI void GLUTAPIENTRY glutMouseFunc(void (GLUTCALLBACK *func)(int button, int state, int x, int y))
{
	if (current != NULL) current->mouse = func;
}

GLUTAPI void GLUTAPIENTRY glutMotionFunc(void (GLUTCALLBACK *func)(int x, int y))
{
	if (current != NULL) current->motion = func;
}

GLUTAPI void GLUTAPIENTRY glutEntryFunc(void (GLUTCALLBACK *func)(int state))
{
	if (current != NULL) current->entry = func;
}

GLUTAPI void GLUTAPIENTRY glutPassiveMotionFunc(void (GLUTCALLBACK *func)(int x, int y))
{
	if (current != NULL) current->passive_motion = func;
}

GLUTAPI void GLUTAPIENTRY glutVisibilityFunc(void (GLUTCALLBACK *func)(int state))
{
	if (current != NULL) current->visibility = func;
}

GLUTAPI void GLUTAPIENTRY glutIdleFunc(void (GLUTCALLBACK *func)(void))
{
	if (current != NULL) current->idle  = func;
}

GLUTAPI void GLUTAPIENTRY glutSpecialFunc(void (GLUTCALLBACK *func)(int key, int x, int y))
{
	if (current != NULL) current->special = func;
}

GLUTAPI int GLUTAPIENTRY glutGetModifiers(void)
{
	int ret = 0;
	if ((modifiers & KEY_MODIFIER_SHIFT) != 0) ret |= GLUT_ACTIVE_SHIFT;
	if ((modifiers & KEY_MODIFIER_CTRL ) != 0) ret |= GLUT_ACTIVE_CTRL;
	if ((modifiers & KEY_MODIFIER_ALT  ) != 0) ret |= GLUT_ACTIVE_ALT;
	return ret;
}

GLUTAPI int GLUTAPIENTRY glutGet(GLenum type)
{
	switch (type)
	{
		case GLUT_ELAPSED_TIME:
			return (syscall_get_tick() - start_tick) * 10;
		case GLUT_WINDOW_X:
			return current == NULL ? 0 : current->get_X();
		case GLUT_WINDOW_Y:
			return current == NULL ? 0 : current->get_Y();
		case GLUT_WINDOW_WIDTH:
			return current == NULL ? 0 : current->get_ClientSize().Width;
		case GLUT_WINDOW_HEIGHT:
			return current == NULL ? 0 : current->get_ClientSize().Height;
		default:
			WARN_NOT_IMPLEMENTED("glutGet(%d)", type);
			break;
	}
	return 0;
}

GLUTAPI void GLUTAPIENTRY glutPostRedisplay(void)
{
	if (current != NULL) current->PostRedisplay();
}

GLUTAPI void GLUTAPIENTRY glutMainLoop(void)
{
	FOREACH_AL (_P<_glut_window>, w, windows)
	{
		w->Show();
	} END_FOREACH
	
	while (!exited && windows->get_Count() > 0)
	{
		Application::DoEvents();
		if (exited) break;
		
		bool idle = !MonAPI::Message::exist(), wait = idle;
		FOREACH_AL (_P<_glut_window>, w, windows)
		{
			w->Reshape();
			if (idle && w->idle != NULL)
			{
				glutSetWindow(w->get_ID());
				(*w->idle)();
				wait = false;
				if (exited) break;
			}
			w->Redisplay();
		} END_FOREACH
		if (wait && windows->get_Count() > 0)
		{
			syscall_mthread_yield_message();
		}
	}
	
	FOREACH_AL (_P<_glut_window>, w, windows)
	{
		if (w->get_Visible()) w->Dispose();
	} END_FOREACH
	Application::Dispose();
	if (isInDLL(__CTOR_LIST__)) invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
}

GLUTAPI int GLUTAPIENTRY glutExtensionSupported(const char *name)
{
	return 0;
}

GLUTAPI void GLUTAPIENTRY glutReshapeWindow(int width, int height)
{
	if (current != NULL) current->Reshape(width, height);
}

GLUTAPI int GLUTAPIENTRY glutCreateSubWindow(int win, int x, int y, int width, int height)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutCreateSubWindow");
	return 0;
}

GLUTAPI void GLUTAPIENTRY glutSetColor(int ndx, GLfloat red, GLfloat green, GLfloat blue)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutSetColor");
}

GLUTAPI void GLUTAPIENTRY glutFullScreen(void)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutFullScreen");
}

GLUTAPI void GLUTAPIENTRY glutSetCursor(int cursor)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutSetCursor");
}

GLUTAPI void GLUTAPIENTRY glutDestroyMenu(int menu)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutDestroyMenu");
}

GLUTAPI int GLUTAPIENTRY glutCreateMenu(void (GLUTCALLBACK *func)(int))
{
	WARN_NOT_IMPLEMENTED("%s()", "glutDestroyMenu");
	return 0;
}

GLUTAPI void GLUTAPIENTRY glutAddMenuEntry(const char *label, int value)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutAddMenuEntry");
}

GLUTAPI void GLUTAPIENTRY glutAttachMenu(int button)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutAttachMenu");
}

GLUTAPI void GLUTAPIENTRY glutDetachMenu(int button)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutDetachMenu");
}

GLUTAPI void GLUTAPIENTRY glutUseLayer(GLenum layer)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutUseLayer");
}

GLUTAPI int GLUTAPIENTRY glutLayerGet(GLenum type)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutLayerGet");
	return 0;
}

GLUTAPI void GLUTAPIENTRY glutEstablishOverlay(void)
{
	WARN_NOT_IMPLEMENTED("%s()", "glutEstablishOverlay");
}
