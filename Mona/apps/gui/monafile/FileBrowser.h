#ifndef __MONAFILE_FILEBROWSER_H__
#define __MONAFILE_FILEBROWSER_H__

#include <gui/System/Mona/Forms/Form.h>

class FileBrowser : public System::Mona::Forms::Control
{
private:
	String path;
	monapi_cmemoryinfo* files;
	
public:
	FileBrowser();
	virtual ~FileBrowser() {}
	
	void Dispose();
	void ReadDirectory(String path);
	int GetIcon(String fileName);
	
protected:
	virtual void OnPaint();
};

#endif  // __MONAFILE_FILEBROWSER_H__
