#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <gui/System/Array.h>
#include "ELFServer.h"
#include "elfparser.h"
#include "ProcessManager.h"

using namespace MonAPI;

int ExecuteProcess(dword parent, monapi_cmemoryinfo* mi, const CString& path, const CString& name, CommandOption* option, bool prompt)
{
    ELFParser parser;
    bool ok = parser.set(mi->Data, mi->Size);
    if (!ok)
    {
        if (prompt) printf("%s: file type is not ELF!\n", SVR);
        return 3;
    }

    int type = parser.getType();
    if (type != ELFParser::TYPE_RELOCATABLE && type != ELFParser::TYPE_EXECUTABLE)
    {
        if (prompt) printf("%s: file type is not supported!\n", SVR);
        return 3;
    }

    int result = parser.parse();
    if (result != 0)
    {
        if (prompt) printf("%s: can not parse!\n", SVR);
        return 3;
    }

    dword imageSize = parser.getImageSize();
    _A<byte> image(imageSize);
    if (!parser.load(image.get()))
    {
        if (prompt) printf("%s: load failed!\n", SVR);
        return 3;
    }

    LoadProcessInfo info;
    info.image = image.get();
    info.size = image.get_Length();
    info.entrypoint = parser.getEntryPoint();
    info.path = path;
    info.name = name;
    info.list = option;

    addProcessInfo(name);
    int ret = syscall_load_process_image(&info);
    addProcessInfo(parent, name, path);

    if (prompt)
    {
        switch(ret)
        {
            case 4:
                  printf("%s: Shared Memory error1", SVR);
                  break;
            case 5:
                  printf("%s: Shared Memory error2", SVR);
                  break;
        }
    }

    return ret;
}

static CString GetFileName(const CString& path)
{
    int p = path.lastIndexOf('/');
    if (p < 0) return path;

    p++;
    return path.substring(p, path.getLength() - p);
}

int ExecuteFile(dword parent, const CString& commandLine, bool prompt)
{
    /* list initilize */
    CommandOption list;
    list.next = NULL;

    CommandOption* option = NULL;
    CString path;
    _A<CString> args = commandLine.split(' ');
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        if (path == NULL)
        {
            path = arg.toUpper();
            continue;
        }

        option = new CommandOption;
        strncpy(option->str, arg, sizeof(option->str));
        option->next = list.next;
        list.next = option;
    }

    monapi_cmemoryinfo* mi = NULL;
    if (path.endsWith(".EL2"))
    {
        mi = monapi_call_file_decompress_bz2_file(path, prompt ? 1 : 0);
    }
    else
    {
        mi = monapi_call_file_read_data(path, prompt ? 1 : 0);
    }

    int result = 1;
    if (mi == NULL)
    {
        if (prompt) printf("%s: can not find command\n", SVR);
    }
    else
    {
        result = ExecuteProcess(parent, mi, path, GetFileName(path), &list, prompt);

        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    for (option = list.next; option; option = option->next)
    {
        delete option;
    }

    return result;
}

void MessageLoop()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg)) continue;

        switch (msg.header)
        {
            case MSG_ELF_EXECUTE_FILE:
            {
                int result = ExecuteFile(msg.from, msg.str, msg.arg1 != 0);
                Message::reply(&msg, result);
                break;
            }
            default:
                if (handleMessage(msg)) break;
                break;
        }
    }
}

int MonaMain(List<char*>* pekoe)
{
    if (Message::send(Message::lookupMainThread("INIT"), MSG_SERVER_START_OK) != 0)
    {
        printf("%s: INIT error\n", SVR);
        exit(1);
    }

    MessageLoop();

    return 0;
}
