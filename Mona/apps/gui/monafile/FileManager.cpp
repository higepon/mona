// This file's encoding is UTF-8.

#include "Icon.h"
#include <gui/System/Mona/Forms/Application.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/CString.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern _P<Bitmap> icons;

static bool ExistsProcess(const MonAPI::CString& self)
{
	syscall_set_ps_dump();
	PsInfo info;

	bool ret = false;
	dword tid = syscall_get_tid();

	while (syscall_read_ps_dump(&info) == 0)
	{
		if (!ret && self == info.name && tid != info.tid) ret = true;
	}
	if (ret) printf("%s: already has executed!\n", (const char*)self);

	return ret;
}

int ProcessStart(const String& file)
{
	int len = file.get_Length();
	_A<char> elf(len + 1);
	for (int i = 0; i < len; i++)
	{
		wchar ch = file[i];
		elf[i] = ch < 128 ? ch : '?';
	}
	elf[len] = '\0';
	return monapi_call_process_execute_file(elf.get(), MONAPI_FALSE);
}

class FileManager
{
public:
	static void FileManager::Main(_A<String> args)
	{
		if (ExistsProcess("MONAFILE.EX2")) return;
		
		String bundlePath = MonAPI::System::getBundlePath();
		icons = new Bitmap(bundlePath + "/ICONS.BM2");
		int len = icons->get_Width() * icons->get_Height();
		Color* ptr = icons->get();
		Color trans = TRANSPARENT_COLOR, ept = Color::get_Empty();
		for (int i = 0; i < len; i++, ptr++)
		{
			if (*ptr == trans) *ptr = ept;
		}
		
		_P<Icon> root = new Icon();
		root->set_Text("/");
		root->set_Icon(Icons_Floppy);
		root->Show();
		
		_P<Icon> terminal = new Icon();
		terminal->set_Location(Point(0, 64));
		terminal->set_Text("た～みなる");
		terminal->set_Icon(Icons_Terminal);
		terminal->set_Target("/APPS/MONAFRMS/SHELL.EX2");
		terminal->Show();
		
		Application::Run();
	}
};

SET_MAIN_CLASS(FileManager)
