// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Drawing/Font.h>

using namespace System;
using namespace System::Drawing;

#define BASE Control

namespace System { namespace Mona { namespace Forms
{
	Label::Label()
	{
	}
	
	Label::~Label()
	{
	}
	
	void Label::OnPaint()
	{
		_P<Graphics> g = this->CreateGraphics();
		g->FillRectangle(this->get_BackColor(), 0, 0, this->get_Width(), this->get_Height());
		g->DrawString(this->get_Text(), Control::get_DefaultFont(), this->get_ForeColor(), 0, 0);
		g->Dispose();
	}
}}}
