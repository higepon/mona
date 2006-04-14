// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "SheetTab.h"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Mona::Forms;

SheetTab::SheetTab() : selectedIndex(-1)
{
	this->tabs = new ArrayList<String>();
}

SheetTab::~SheetTab()
{
}

void SheetTab::Add(String name)
{
	this->tabs->Add(name);
	this->Refresh();
}

void SheetTab::AddSelect(String name)
{
	this->tabs->Add(name);
	this->set_SelectedIndex(this->tabs->get_Count() - 1);
}

void SheetTab::set_SelectedIndex(int value)
{
	if (value == this->selectedIndex || value < 0 || value >= this->tabs->get_Count()) return;
	
	this->selectedIndex = value;
	this->Refresh();
	this->OnSelectedIndexChanged(EventArgs::get_Empty());
}

int SheetTab::GetTabPosition(int tab)
{
	int ret = 0;
	_P<Font> f = Control::get_DefaultFont();
	for (int i = 0; i < tab; i++)
	{
		ret += 16 + f->GetSize(this->tabs->get_Item(i)).Width;
	}
	return ret;
}

int SheetTab::GetTabIndex(int x)
{
	int tx = 0, len = this->tabs->get_Count();
	_P<Font> f = Control::get_DefaultFont();
	for (int i = 0; i < len; i++)
	{
		int w = f->GetSize(this->tabs->get_Item(i)).Width;
		if (tx + 4 <= x && x < tx + w + 20) return i;
		tx += 16 + w;
	}
	return -1;
}

void SheetTab::Draw(_P<Graphics> g, int index)
{
	_P<Font> f = Control::get_DefaultFont();
	Color b = Color::get_Black(), w = Color::get_White(), bc = this->get_BackColor();
	int x = this->GetTabPosition(index), height = this->get_Height();
	String s = this->tabs->get_Item(index);
	int width = f->GetSize(s).Width;
	bool sel = index == this->selectedIndex;
	Color back = sel ? w : bc;
	for (int i = 0; i < 8; i++)
	{
		int x1 = x + i, x2 = x + width + 23 - i;
		if (i > 0)
		{
			g->DrawLine(back, x1, 0, x1, i * 2 - 1);
			g->DrawLine(back, x2, 0, x2, i * 2 - 1);
		}
		g->SetPixel(x1, i * 2, b);
		g->SetPixel(x1, i * 2 + 1, b);
		g->SetPixel(x2, i * 2, b);
		g->SetPixel(x2, i * 2 + 1, b);
	}
	g->FillRectangle(back, x + 8, 0, width + 8, height - 1);
	g->DrawLine(b, x + 8, height - 1, x + width + 15, height - 1);
	g->DrawString(s, f, b, x + 12, sel ? 1 : 2);
	if (!sel) g->DrawLine(b, x, 0, x + width + 23, 0);
}

void SheetTab::OnPaint()
{
	int count = this->tabs->get_Count();
	if (count == 0) return;
	
	_P<Graphics> g = this->CreateGraphics();
	for (int i = count - 1; i >= 0; i--)
	{
		if (i != this->selectedIndex) this->Draw(g, i);
	}
	this->Draw(g, this->selectedIndex);
	g->Dispose();
}

void SheetTab::OnMouseDown(_P<MouseEventArgs> e)
{
	Control::OnMouseDown(e);
	if (e->Button != 1) return;
	
	int index = this->GetTabIndex(e->X);
	if (index == -1) return;
	
	this->set_SelectedIndex(index);
}

void SheetTab::OnSelectedIndexChanged(_P<EventArgs> e)
{
	this->raise_SelectedIndexChanged(this, e);
}
