// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi.h>
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
		g->DrawString(this->get_Text(), Control::get_DefaultFont(), Color::get_Black(), 0, 0);
		g->Dispose();
	}
}}}
