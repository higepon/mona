// This file's encoding is UTF-8.

#include "FileWindow.h"
#include <gui/System/Mona/Forms/ControlPaint.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

FileBrowser::FileBrowser()
{
	this->set_BackColor(Color::get_White());
	this->offset = Point(2, 2);
}

void FileBrowser::OnPaint()
{
	_P<Graphics> g = Graphics::FromImage(this->buffer);
	ControlPaint::DrawSunken(g, 0, 0, this->get_Width(), this->get_Height());
	g->DrawString("実装されていません!! ○|￣|＿", Control::get_DefaultFont(), Color::get_Black(), 20, 20);
	g->Dispose();
}
