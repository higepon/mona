// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Drawing/Rectangle.h>
#define _R ::System::Drawing::Rectangle

#include <monapi/messages.h>
#include "window.h"
#include "screen.h"
#include "image.h"

#define DEFAULT_WIDTH  240
#define DEFAULT_HEIGHT 160

using namespace MonAPI;

extern guiserver_bitmap* screen_buffer, * wallpaper;
extern int mouse_x, mouse_y;

static HList<guiserver_window*> windows;
static int start_pos = 0;
static HList<guiserver_window*> captures;

guiserver_window* CreateWindow()
{
	dword handle = MemoryMap::create(sizeof(guiserver_window));
	if (handle == 0) return NULL;
	
	guiserver_window* ret = (guiserver_window*)MemoryMap::map(handle);
	if (ret == NULL) return NULL;
	
	ret->Handle   = handle;
	ret->Parent   = 0;
	ret->Owner    = 0;
	ret->ThreadID = 0;
	ret->X        = start_pos;
	ret->Y        = start_pos;
	ret->Width    = DEFAULT_WIDTH;
	ret->Height   = DEFAULT_HEIGHT;
	ret->OffsetX  = 0;
	ret->OffsetY  = 0;
	ret->Opacity  = 255;
	ret->Visible  = false;
	ret->Flags    = 0;
	ret->TransparencyKey = 0;
	ret->BufferHandle = ret->FormBufferHandle = 0;
	ret->__reserved1 = NULL;
	windows.add(ret);
	
	start_pos += 32;
	Screen* scr = GetDefaultScreen();
	if (start_pos + DEFAULT_WIDTH >= scr->getWidth()
		&& start_pos + DEFAULT_HEIGHT >= scr->getHeight())
	{
		start_pos = 0;
	}

	return ret;
}

guiserver_window* GetWindowPointer(dword handle)
{
	int size = windows.size();
	for (int i = 0; i < size; i++)
	{
		if (windows[i]->Handle == handle) return windows[i];
	}
	return NULL;
}

bool DisposeWindow(dword handle)
{
	int size_c = captures.size();
	for (int i = 0; i < size_c; i++)
	{
		if (captures[i]->Handle == handle)
		{
			captures.removeAt(i);
			break;
		}
	}
	int size_w = windows.size();
	for (int i = 0; i < size_w; i++)
	{
		if (windows[i]->Handle == handle)
		{
			windows.removeAt(i);
			MemoryMap::unmap(handle);
			return true;
		}
	}
	return false;
}

void DisposeWindowFromThreadID(dword tid)
{
	for (int i = 0; i < captures.size(); i++)
	{
		if (captures[i]->ThreadID == tid)
		{
			captures.removeAt(i);
			i--;
		}
	}
	for (int i = 0; i < windows.size(); i++)
	{
		guiserver_window* w = windows[i];
		if (w->ThreadID == tid)
		{
			if (w->FormBufferHandle != 0)
			{
				w->Visible = false;
				DrawWindow(w);
			}
			windows.removeAt(i);
			MemoryMap::unmap(w->Handle);
			i--;
		}
	}
}

void DrawWindow(guiserver_window* w, bool draw_screen /*= true*/)
{
	if (w == NULL || w->FormBufferHandle == 0) return;
	
	DrawImage(screen_buffer, wallpaper, w->X, w->Y, w->X, w->Y, w->Width, w->Height);
	_R r(w->X, w->Y, w->Width, w->Height);
	int size = windows.size();
	for (int i = 0; i < size; i++)
	{
		guiserver_window* ww = windows[i];
		if (!ww->Visible || ww->FormBufferHandle == 0) continue;
		
		_R rr(ww->X, ww->Y, ww->Width, ww->Height);
		rr.Intersect(r);
		if (r.Width == 0 || rr.Height == 0) continue;
		
		if (ww->__reserved1 == NULL)
		{
			ww->__reserved1 = GetBitmapPointer(ww->FormBufferHandle);
			if (ww->__reserved1 == NULL) return;
		}
		DrawImage(screen_buffer, ww->__reserved1, rr.X, rr.Y, rr.X - ww->X, rr.Y - ww->Y, rr.Width, rr.Height, ww->TransparencyKey, ww->Opacity);
	}
	if (!draw_screen) return;
	
	bool mouse = _R(mouse_x - 4, mouse_y - 4, 16, 16).IntersectsWith(r);
	if (mouse) monapi_call_mouse_set_cursor(MONAPI_FALSE);
	DrawScreen(w->X, w->Y, w->Width, w->Height);
	if (mouse) monapi_call_mouse_set_cursor(MONAPI_TRUE);
}

void MoveWindow(guiserver_window* w, int x, int y)
{
	if (w == NULL || !w->Visible || w->FormBufferHandle == 0 || (w->X == x && w->Y == y)) return;
	
	w->Visible = false;
	DrawWindow(w, false);
	int xx = w->X, yy = w->Y;
	w->Visible = true;
	w->X = x;
	w->Y = y;
	monapi_call_mouse_set_cursor(MONAPI_FALSE);
	DrawWindow(w);
	DrawScreen(xx, yy, w->Width, w->Height);
	monapi_call_mouse_set_cursor(MONAPI_TRUE);
}

guiserver_window* GetTargetWindow(int x, int y)
{
	int size = captures.size();
	if (size > 0)
	{
		return captures[size - 1];
	}
	
	for (int i = windows.size() - 1; i >= 0; i--)
	{
		guiserver_window* w = windows[i];
		if (w->FormBufferHandle == 0) continue;
		
		_R r(w->X, w->Y, w->Width, w->Height);
		if (!r.Contains(x, y)) continue;
		
		if (w->__reserved1 == NULL)
		{
			w->__reserved1 = GetBitmapPointer(w->FormBufferHandle);
			if (w->__reserved1 == NULL) continue;
		}
		unsigned int c = w->__reserved1->Data[(x - w->X) + (y - w->Y) * w->Width];
		if (c != 0 && c != w->TransparencyKey) return w;
	}
	
	return NULL;
}

bool WindowHandler(MessageInfo* msg)
{
	switch (msg->header)
	{
		case MSG_GUISERVER_CREATEWINDOW:
		{
			guiserver_window* w = CreateWindow();
			w->ThreadID = msg->from;
			Message::reply(msg, w->Handle);
			break;
		}
		case MSG_GUISERVER_DISPOSEWINDOW:
			DisposeWindow(msg->arg1);
			Message::reply(msg);
			break;
		case MSG_GUISERVER_DRAWWINDOW:
			DrawWindow(GetWindowPointer(msg->arg1));
			Message::reply(msg);
			break;
		case MSG_GUISERVER_MOVEWINDOW:
			MoveWindow(GetWindowPointer(msg->arg1), (int)msg->arg2, (int)msg->arg3);
			Message::reply(msg);
			break;
		case MSG_MOUSE_INFO:
		{
			mouse_x = msg->arg1;
			mouse_y = msg->arg2;
			guiserver_window* target = GetTargetWindow(mouse_x, mouse_y);
			if (target != NULL)
			{
				if (Message::send(target->ThreadID, msg) != 0)
				{
					DisposeWindowFromThreadID(target->ThreadID);
				}
			}
			break;
		}
		case MSG_GUISERVER_MOUSECAPTURE:
		{
			guiserver_window* w = GetWindowPointer(msg->arg1);
			if (w != NULL)
			{
				if (msg->arg2 == 0)
				{
					captures.remove(w);
				}
				else
				{
					captures.add(w);
				}
			}
			Message::reply(msg);
			break;
		}
		default:
			return false;
	}
	return true;
}
