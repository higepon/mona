// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __MONADAT_SHEETTAB_H__
#define __MONADAT_SHEETTAB_H__

#include <gui/System/Collections/ArrayList.h>
#include <gui/System/Mona/Forms/Control.h>

class SheetTab : public System::Mona::Forms::Control
{
private:
	_P<System::Collections::ArrayList<System::String> > tabs;
	int selectedIndex;
	
public:
	SheetTab();
	virtual ~SheetTab();
	void Add(System::String name);
	void AddSelect(System::String name);
	inline int get_SelectedIndex() { return this->selectedIndex; }
	void set_SelectedIndex(int value);
	inline int get_Count() { return this->tabs->get_Count(); }
	int GetTabPosition(int tab);
	int GetTabIndex(int x);
	
protected:
	void Draw(_P<System::Drawing::Graphics> g, int index);
	virtual void OnPaint();
	virtual void OnMouseDown(_P<System::Mona::Forms::MouseEventArgs> e);
	virtual void OnSelectedIndexChanged(_P<System::EventArgs> e);
	
	DECLARE_EVENT(System::IEventHandler, SelectedIndexChanged, _P<System::EventArgs>)
};

#endif  // __MONADAT_SHEETTAB_H__
