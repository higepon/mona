#include "Icon.h"
#include "FileWindow.h"
#include <monapi.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern dword __gui_server;
extern int ProcessStart(const String& file);

_P<Bitmap> icons;
_P<FileWindow> win;

Icon::Icon()
{
	this->set_BackColor(TRANSPARENT_COLOR);
	this->set_TransparencyKey(TRANSPARENT_COLOR);
	this->offset = Point::get_Empty();
	this->set_Size(Size(64, 48));
}

void Icon::Create()
{
	Form::Create();
	this->_object->Flags |= WINDOWFLAGS_BOTTOMMOST | WINDOWFLAGS_NOBORDER;
	this->_object->__internal2 = true;
}

void Icon::set_Icon(Icons icon)
{
	if (this->icon == icon) return;
	
	this->icon = icon;
	if (this->get_Visible()) this->Refresh();
}

void Icon::set_Target(String target)
{
	this->target = target;
}

void Icon::OnPaint()
{
	_P<Graphics> g = this->CreateGraphics();
	g->DrawImage(icons, 16, 0, Rectangle(0, 32 * (int)this->icon, 32, 32));
	
	_P<Font> f = Control::get_DefaultFont();
	Size sz = g->MeasureString(this->get_Text(), f);
	int x = (this->get_Width() - sz.Width) / 2;
	g->DrawString(this->get_Text(), f, Color::get_Black(), x + 1, 34);
	g->DrawString(this->get_Text(), f, Color::get_Black(), x - 1, 34);
	g->DrawString(this->get_Text(), f, Color::get_Black(), x, 32);
	g->DrawString(this->get_Text(), f, Color::get_Black(), x, 35);
	g->DrawString(this->get_Text(), f, Color::get_White(), x, 34);
	
	g->Dispose();
}

Control::NCState Icon::NCHitTest(int x, int y)
{
	return NCState_TitleBar;
}

void Icon::OnNCMouseUp(_P<MouseEventArgs> e)
{
	Point p = this->get_Location();
	Form::OnNCMouseUp(e);
	if (e->Button != 1 || p != this->get_Location()) return;
	
	int x = this->get_X(), y = this->get_Y(), w = this->get_Width(), h = this->get_Height();
	if (this->target.get_Length() > 0)
	{
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_EXPANSIONEFFECT,
			MAKE_DWORD(x + w / 2, y + h / 2), MAKE_DWORD(x, y), MAKE_DWORD(w, h));
		ProcessStart(this->target);
	}
	else if (win == NULL || !win->get_Visible())
	{
		win = new FileWindow();
		MonAPI::Message::sendReceive(NULL, __gui_server, MSG_GUISERVER_EXPANSIONEFFECT,
			MAKE_DWORD(x + w / 2, y + h / 2),
			MAKE_DWORD(win->get_X(), win->get_Y()),
			MAKE_DWORD(win->get_Width(), win->get_Height()));
		win->Show();
	}
}
