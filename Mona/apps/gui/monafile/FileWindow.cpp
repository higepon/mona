// This file's encoding is UTF-8.

#include "FileWindow.h"
#include <gui/System/Mona/Forms/ControlPaint.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

FileWindow::FileWindow()
{
	this->set_Bounds(Rectangle(80, 80, 510, 340));
	this->set_Text("ふぁいるまね～じゃ");
	this->set_Opacity(0.8);
	
	this->browser = new FileBrowser();
	this->browser->set_Size(this->get_ClientSize());
	this->get_Controls()->Add(this->browser.get());
}

void FileWindow::Create()
{
	Form::Create();
	this->_object->__internal2 = true;
}

void FileWindow::set_Directory(String path)
{
	this->set_Text(path);
	this->browser->ReadDirectory(path);
}
