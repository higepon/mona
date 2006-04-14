#ifndef __MONAFILE_FILEBROWSER_H__
#define __MONAFILE_FILEBROWSER_H__

#include <monapi/cmemoryinfo.h>
#include <gui/System/Mona/Forms/Form.h>

class FileBrowser : public System::Mona::Forms::Control
{
private:
	System::String path;
	monapi_cmemoryinfo* files;
	int target, clickTarget;
	int skip;
	
public:
	FileBrowser();
	virtual ~FileBrowser() {}
	
	void Dispose();
	void ReadDirectory(System::String path);
	int GetTarget(int ex, int ey);
	void Open(int target);
	
protected:
	virtual void OnPaint();
	virtual void OnMouseMove(_P<System::Mona::Forms::MouseEventArgs> e);
	virtual void OnMouseDown(_P<System::Mona::Forms::MouseEventArgs> e);
	virtual void OnMouseUp(_P<System::Mona::Forms::MouseEventArgs> e);
	virtual void OnMouseLeave(_P<System::EventArgs> e);
};

#endif  // __MONAFILE_FILEBROWSER_H__
