#include "Icon.h"
#include "FileWindow.h"
#include <gui/System/Collections/ArrayList.h>
#include <monapi.h>

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern dword gui_server;
extern int ProcessStart(const String& file);

_P<Bitmap> icons;
_P<ArrayList<_P<FileWindow> > > windows;

Icon::Icon()
{
	this->set_BackColor(TRANSPARENT_COLOR);
	this->set_TransparencyKey(TRANSPARENT_COLOR);
	this->offset = Point::get_Empty();
	this->set_Size(Size(ARRANGE_WIDTH, 49));
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
	Icon::DrawIcon(g, this->get_Text(), this->icon, 0, 0, true, true);
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
	int cx = x + w / 2, cy = y + h / 2;
	if (this->target.StartsWith("open "))
	{
		this->Open(cx, cy, this->target.Substring(5, this->target.get_Length() - 5));
	}
	else if (this->target.get_Length() > 0)
	{
		Icon::ExpansionEffect(cx, cy);
		ProcessStart(this->target);
	}
	else
	{
		this->Open(cx, cy, this->get_Text());
	}
}

void Icon::ExpansionEffect(int x, int y)
{
	MonAPI::Message::sendReceive(NULL, gui_server, MSG_GUISERVER_EXPANSIONEFFECT,
		MAKE_DWORD(x, y), MAKE_DWORD(x - 32, y - 32), MAKE_DWORD(64, 64));
}

bool Icon::Open(int cx, int cy, String dir)
{
	if (windows == NULL)
	{
		windows = new ArrayList<_P<FileWindow> >;
	}
	else
	{
		for (int i = 0; i < windows->get_Count(); i++)
		{
			_P<FileWindow> win = windows->get_Item(i);
			if (!win->get_Visible())
			{
				windows->RemoveAt(i);
				i--;
			}
			else if (win->get_Text() == dir)
			{
				return false;
			}
		}
	}
	
	_P<FileWindow> win = new FileWindow();
	windows->Add(win);
	MonAPI::Message::sendReceive(NULL, gui_server, MSG_GUISERVER_EXPANSIONEFFECT,
		MAKE_DWORD(cx, cy),
		MAKE_DWORD(win->get_X(), win->get_Y()),
		MAKE_DWORD(win->get_Width(), win->get_Height()));
	win->Show();
	win->set_Directory(dir);
	return true;
}

Size Icon::DrawIcon(_P<Graphics> g, String name, Icons icon, int x, int y, bool emboss, bool selection)
{
	g->DrawImage(icons, x + (ARRANGE_WIDTH - 32) / 2, y, Rectangle(0, 32 * (int)icon, 32, 32));
	
	_P<Font> f = Control::get_DefaultFont();
	Size ret = g->MeasureString(name, f);
	x += (ARRANGE_WIDTH - ret.Width) / 2;
	Color black = Color::get_Black(), white = Color::get_White();
	int r = x + ret.Width - 1, ly = y + 34 + ret.Height + 1;
	if (emboss)
	{
		g->DrawString(name, f, black, x + 1, y + 34);
		g->DrawString(name, f, black, x - 1, y + 34);
		g->DrawString(name, f, black, x, y + 33);
		g->DrawString(name, f, black, x, y + 35);
		g->DrawString(name, f, white, x, y + 34);
		if (selection)
		{
			g->DrawLine(black, x, ly - 1, r, ly - 1);
			g->DrawLine(white, x, ly    , r, ly    );
			g->DrawLine(black, x, ly + 1, r, ly + 1);
			g->SetPixel(x - 1, ly, black);
			g->SetPixel(r + 1, ly, black);
		}
	}
	else
	{
		g->DrawString(name, f, black, x, y + 34);
		if (selection)
		{
			g->DrawLine(black, x, ly, r, ly);
		}
	}
	return ret;
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
	
	if (fn == "KERNEL.BIN" || fn == "LOADER.BIN")
	{
		return Icons_Kernel;
	}
	else if (fn == "SHELL.EX5")
	{
		return Icons_Terminal;
	}
	else if (ext == ".BZ5")
	{
		return Icons_Archive;
	}
	else if (ext == ".BIN" || ext == ".BN2" || ext == ".BN5"
		|| ext == ".ELF" || ext == ".EL2" || ext == ".EL5"
		|| ext == ".EXE" || ext == ".EX2" || ext == ".EX5")
	{
		return Icons_Executable;
	}
	else if (ext == ".BMP" || ext == ".BM2" || ext == ".BM5" || ext == ".JPG")
	{
		return Icons_Picture;
	}
	else if (ext == ".TXT" || ext == ".MSH" || ext == ".CFG" || ext == ".INI")
	{
		return Icons_Text;
	}
	return Icons_File;
}
