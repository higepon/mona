// This file's encoding is UTF-8.

#include "FileWindow.h"
#include "Icon.h"
#include <gui/System/Mona/Forms/ControlPaint.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern _P<Bitmap> icons;

FileBrowser::FileBrowser()
{
	this->set_BackColor(Color::get_White());
	this->offset = Point(2, 2);
	this->files = NULL;
}

void FileBrowser::Dispose()
{
	Control::Dispose();
	if (this->files == NULL) return;
	
	monapi_cmemoryinfo_dispose(this->files);
	monapi_cmemoryinfo_delete(this->files);
	this->files = NULL;
}

void FileBrowser::ReadDirectory(String path)
{
	this->path = path;
	int len = path.get_Length();
	char* buf = new char[len + 1];
	for (int i = 0; i < len; i++)
	{
		wchar ch = path[i];
		buf[i] = ch < 128 ? (char)ch : '?';
	}
	buf[len] = '\0';
	if (this->files != NULL)
	{
		monapi_cmemoryinfo_dispose(this->files);
		monapi_cmemoryinfo_delete(this->files);
	}
	this->files = monapi_call_file_read_directory(buf, MONAPI_TRUE);
	delete [] buf;
	if (this->files == NULL) return;
	
	if (this->get_Visible()) this->Refresh();
}

void FileBrowser::OnPaint()
{
	_P<Graphics> gf = Graphics::FromImage(this->buffer);
	ControlPaint::DrawSunken(gf, 0, 0, this->get_Width(), this->get_Height());
	gf->Dispose();
	if (this->path.get_Length() < 1) return;
	
	_P<Graphics> g = this->CreateGraphics();
	Size sz = this->get_ClientSize();
	int w = sz.Width, h = sz.Height;
	g->FillRectangle(this->get_BackColor(), 0, 0, w, h);
	if (this->files == NULL)
	{
		g->DrawString("ディレクトリが開けません:\n\n" + this->path,
			Control::get_DefaultFont(), Color::get_Black(), 20, 20);
	}
	else
	{
		int x = 8, y = 8;
		int len = *(int*)this->files->Data;
		monapi_directoryinfo* di = (monapi_directoryinfo*)&this->files->Data[sizeof(int)];
		for (int i = 0; i < len; i++, di++)
		{
			Icon::DrawIcon(g, di->name, Icon::GetIcon(di), x, y, false);
			x += ARRANGE_WIDTH;
			if (x + ARRANGE_WIDTH >= w)
			{
				x = 8;
				y += ARRANGE_HEIGHT;
			}
		}
	}
	g->Dispose();
}
