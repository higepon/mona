// This file's encoding is UTF-8.

#ifndef __MONAFILE_ICON_H__
#define __MONAFILE_ICON_H__

#include <gui/System/Mona/Forms/Form.h>

#define TRANSPARENT_COLOR Color::FromArgb(142, 187, 138)  // 透過色 (DarkSeaGreen)

enum Icons
{
	Icons_Archive,
	Icons_Executable,
	Icons_File,
	Icons_Floppy,
	Icons_Folder,
	Icons_Kernel,
	Icons_Picture,
	Icons_Server,
	Icons_Terminal,
	Icons_Text
};

class Icon : public System::Mona::Forms::Form
{
private:
	Icons icon;
	System::String target;
	
public:
	Icon();
	virtual ~Icon() {}
	
	virtual void Create();
	
	inline Icons get_Icon() { return this->icon; }
	void set_Icon(Icons icon);
	inline System::String get_Target() { return this->target; }
	void set_Target(System::String target);
	
protected:
	virtual void OnPaint();
	virtual System::Mona::Forms::Control::NCState NCHitTest(int x, int y);
	virtual void OnNCMouseUp(_P<System::Mona::Forms::MouseEventArgs> e);
};

#endif  // __MONAFILE_ICON_H__
