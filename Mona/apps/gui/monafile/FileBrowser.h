#ifndef __MONAFILE_FILEBROWSER_H__
#define __MONAFILE_FILEBROWSER_H__

#include <gui/System/Mona/Forms/Form.h>

class FileBrowser : public System::Mona::Forms::Control
{
public:
	FileBrowser();
	virtual ~FileBrowser() {}
	
protected:
	virtual void OnPaint();
};

#endif  // __MONAFILE_FILEBROWSER_H__
