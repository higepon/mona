// This file's encoding is UTF-8.

#include "Icon.h"
#include <gui/System/Mona/Forms/Application.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/CString.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

dword gui_server;
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

static bool DirectoryExists(const char* path)
{
	monapi_cmemoryinfo* files = monapi_file_read_directory(path);
	if (files == NULL) return false;
	
	int size = *(int*)files->Data;
	monapi_cmemoryinfo_dispose(files);
	monapi_cmemoryinfo_delete (files);
	return size > 0;
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
		if (ExistsProcess("MONAFILE.EX5")) return;
		
		gui_server = monapi_get_server_thread_id(ID_GUI_SERVER);
		String bundlePath = MonAPI::System::getBundlePath();
		icons = new Bitmap(bundlePath + "/ICONS.BM5");
		int len = icons->get_Width() * icons->get_Height();
		Color* ptr = icons->get();
		Color trans = TRANSPARENT_COLOR, ept = Color::get_Empty();
		for (int i = 0; i < len; i++, ptr++)
		{
			if (*ptr == trans) *ptr = ept;
		}
		
		int offsetX = 0;
		
		_P<Icon> root = new Icon();
		root->set_Location(Point(0, offsetX));
		root->set_Text("/APPS");
		root->set_Icon(Icons_Floppy);
		root->Show();
		offsetX += 64;
		
		_P<Icon> terminal = new Icon();
		terminal->set_Location(Point(0, offsetX));
		terminal->set_Text("た～みなる");
		terminal->set_Icon(Icons_Terminal);
		terminal->set_Target("/APPS/MONAFRMS/MSHELL.EX5");
		terminal->Show();
		offsetX += 64;
		
		if (DirectoryExists("/APPS/MONAFRMS/MESA"))
		{
			_P<Icon> mesa = new Icon();
			mesa->set_Location(Point(0, offsetX));
			mesa->set_Text("3Dデモ");
			mesa->set_Icon(Icons_Folder);
			mesa->set_Target("open /APPS/MONAFRMS/MESA");
			mesa->Show();
			offsetX += 64;
		}
		
		if (DirectoryExists("/APPS/MONAFRMS/MONADAT.APP"))
		{
			_P<Icon> mesa = new Icon();
			mesa->set_Location(Point(0, offsetX));
			mesa->set_Text("OSを作ろう");
			mesa->set_Icon(Icons_Executable);
			mesa->set_Target("/APPS/MONAFRMS/MONADAT.APP/MONADAT.EX5");
			mesa->Show();
			offsetX += 64;
		}
		
		Application::Run();
	}
};

SET_MAIN_CLASS(FileManager)
