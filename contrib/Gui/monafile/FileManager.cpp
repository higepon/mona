// This file's encoding is UTF-8.

#include "Icon.h"
#include <gui/System/Mona/Forms/Application.h>
#include <monapi.h>
#include <stdio.h>
#include <monapi/messages.h>
#include <monapi/CString.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

uint32_t gui_server;
extern _P<System::Drawing::Bitmap> icons;

static bool ExistsProcess(const MonAPI::CString& self)
{
    syscall_set_ps_dump();
    PsInfo info;

    bool ret = false;
    uint32_t tid = syscall_get_tid();

    while (syscall_read_ps_dump(&info) == 0)
    {
        if (!ret && self == info.name && tid != info.tid) ret = true;
    }
    if (ret) printf("%s: already has executed!\n", (const char*)self);

    return ret;
}

static bool DirectoryExists(const char* path)
{
    MonAPI::scoped_ptr<MonAPI::SharedMemory> files(monapi_file_read_directory(path));
    if (files.get() == NULL) return false;

    int size = *(int*)files->data();
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
    return monapi_process_execute_file(elf.get(), MONAPI_FALSE);
}

class FileManager
{
public:
    static void Main(_A<String> args)
    {
        if (ExistsProcess("MONAFILE.EX5")) return;
        const char* MAP_FILE_PATH = "/APPS/MONAFILE.MAP";
        uint32_t pid = syscall_get_pid();
        intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
        if (ret != M_OK) {
            monapi_warn("syscall_stack_trace_enable error %d\n", ret);
            exit(-1);
        }
        if (monapi_name_whereis("/servers/gui", gui_server) != M_OK) {
            monapi_fatal("server not found");
        }

        String bundlePath = MonAPI::System::getBundlePath();
        icons = new System::Drawing::Bitmap(bundlePath + "/ICONS.BMP");
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
        root->set_Text("/APPS/MONAGUI");
        root->set_Icon(Icons_Floppy);
        root->Show();
        offsetX += 64;

//      _P<Icon> terminal = new Icon();
//      terminal->set_Location(Point(0, offsetX));
//      terminal->set_Text("Terminal");
//      terminal->set_Icon(Icons_Terminal);
//      terminal->set_Target("/APPS/MONAFRMS/MSHELL.EX5");
//      terminal->Show();
//      offsetX += 64;

        if (DirectoryExists("/APPS/MONAFRMS/MESA"))
        {
            _P<Icon> mesa = new Icon();
            mesa->set_Location(Point(0, offsetX));
            mesa->set_Text("3D Demos");
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
//      if (DirectoryExists("/MUSIC"))
//      {
//          _P<Icon> mesa = new Icon();
//          mesa->set_Location(Point(0, offsetX));
//          mesa->set_Text("HAPPY");
//          mesa->set_Icon(Icons_Player);
//          mesa->set_Target("/MUSIC/HAPPY.EX5 -d /MUSIC");
//          mesa->Show();
//          offsetX += 64;
//      }


        Application::Run();
    }
};

SET_MAIN_CLASS(FileManager)
