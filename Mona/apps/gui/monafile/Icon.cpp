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
	this->set_Size(Size(ARRANGE_WIDTH, 48));
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
	g->FillRectangle(TRANSPARENT_COLOR, 0, 0, this->get_Width(), this->get_Height());
	Icon::DrawIcon(g, this->get_Text(), this->icon, 0, 0, true);
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
		win->set_Directory(this->get_Text());
	}
}

void Icon::DrawIcon(_P<Graphics> g, String name, Icons icon, int x, int y, bool emboss)
{
	g->DrawImage(icons, x + (ARRANGE_WIDTH - 32) / 2, y, Rectangle(0, 32 * (int)icon, 32, 32));
	
	_P<Font> f = Control::get_DefaultFont();
	Size sz = g->MeasureString(name, f);
	x += (ARRANGE_WIDTH - sz.Width) / 2;
	if (emboss)
	{
		g->DrawString(name, f, Color::get_Black(), x + 1, y + 34);
		g->DrawString(name, f, Color::get_Black(), x - 1, y + 34);
		g->DrawString(name, f, Color::get_Black(), x, y + 32);
		g->DrawString(name, f, Color::get_Black(), x, y + 35);
		g->DrawString(name, f, Color::get_White(), x, y + 34);
	}
	else
	{
		g->DrawString(name, f, Color::get_Black(), x, y + 34);
	}
}

Icons Icon::GetIcon(monapi_directoryinfo* di)
{
	String fn = di->name;
	int len = fn.get_Length();
	int p = -1;
	for (int i = len - 1; i >= 0; i--)
	{
		if (fn[i] == '.')
		{
			p = i;
			break;
		}
	}
	String ext;
	if (p >= 0) ext = fn.Substring(p, len - p);

	if ((di->attr & ATTRIBUTE_DIRECTORY) != 0)
	{
		return ext == ".APP" ? Icons_Executable : Icons_Folder;
	}
	
	if (fn == "KERNEL.IMG")
	{
		return Icons_Kernel;
	}
	else if (fn == "GSHELL.EL2")
	{
		return Icons_Terminal;
	}
	else if (ext == ".BZ2")
	{
		return Icons_Archive;
	}
	else if (ext == ".ELF" || ext == ".EL2" || ext == ".APP")
	{
		return Icons_Executable;
	}
	else if (ext == ".BMP" || ext == ".BM2" || ext == ".JPG")
	{
		return Icons_Picture;
	}
	else if (ext == ".SVR")
	{
		return Icons_Server;
	}
	else if (ext == ".TXT" || ext == ".MSH" || ext == ".CFG" || ext == ".INI")
	{
		return Icons_Text;
	}
	return Icons_File;
}
