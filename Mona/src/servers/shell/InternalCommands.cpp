#include "Shell.h"
#include <monapi/messages.h>

using namespace MonAPI;

enum
{
    COMMAND_NONE,
    COMMAND_HELP,
    COMMAND_LS,
    COMMAND_CD,
    COMMAND_CAT,
    COMMAND_CHSH,
    COMMAND_UNAME,
    COMMAND_ECHO,
    COMMAND_CLEAR,
    COMMAND_PS,
    COMMAND_KILL
};

int Shell::isInternalCommand(const CString& command)
{
    CString cmd = command.toLower();
    if (cmd == "help" || cmd == "?")
    {
        return COMMAND_HELP;
    }
    else if (cmd == "ls" || cmd == "dir")
    {
        return COMMAND_LS;
    }
    else if (cmd == "cd")
    {
        return COMMAND_CD;
    }
    else if (cmd == "cat" || cmd == "type")
    {
        return COMMAND_CAT;
    }
    else if (cmd == "chsh")
    {
        return COMMAND_CHSH;
    }
    else if (cmd == "uname" || cmd == "ver")
    {
        return COMMAND_UNAME;
    }
    else if (cmd == "echo")
    {
        return COMMAND_ECHO;
    }
    else if (cmd == "clear" || cmd == "cls")
    {
        return COMMAND_CLEAR;
    }
    else if (cmd == "ps")
    {
        return COMMAND_PS;
    }
    else if (cmd == "kill")
    {
        return COMMAND_KILL;
    }

    return COMMAND_NONE;
}

bool Shell::internalCommandExecute(int command, _A<CString> args)
{
    bool ret = true;
    monapi_call_mouse_set_cursor(0);
    switch (command)
    {
    case COMMAND_HELP:
        printf("* Mona Shell Internal Commands\n");
        printf("HELP/?, LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, ECHO, CLEAR/CLS, PS, KILL\n");
        break;

    case COMMAND_CD:
        {
            if (args.get_Length() < 2)
            {
                this->current = STARTDIR;
                break;
            }
            CString dir = this->mergeDirectory(this->current, args[1]);
            if (syscall_cd(dir) != 0)
            {
                printf("%s: directory not found: %s\n", SVR, (const char*)dir);
                break;
            }
            this->current = dir;
            break;
        }

    case COMMAND_LS:
        {
            if (args.get_Length() < 2)
            {
                printFiles(this->current);
            }
            else
            {
                for (int i = 1; i < args.get_Length(); i++)
                {
                    if (i > 1) printf("\n");
                    CString dir = this->mergeDirectory(this->current, args[i]);
                    printf("%s:\n", (const char*)dir);
                    printFiles(dir);
                }
            }
            break;
        }

    case COMMAND_CAT:
        {
            if (args.get_Length() < 2)
            {
                printf("usage: CAT/TYPE file\n");
                break;
            }

            monapi_cmemoryinfo* mi = monapi_call_file_read_data(args[1], 1);
            if (mi == NULL) break;

            if (mi->Size > 0)
            {
                byte* p = mi->Data;
                bool cr = false;
                for (dword i = 0; i < mi->Size; i++)
                {
                    byte b = mi->Data[i];
                    switch (b)
                    {
                        case '\r':
                            *p++ = '\n';
                            cr = true;
                            break;
                        case '\n':
                            if (!cr) *p++ = '\n';
                            cr = false;
                            break;
                        default:
                            *p++ = b;
                            cr = false;
                            break;
                    }
                }
                *p = 0;
                printf((const char*)mi->Data);
            }
            monapi_cmemoryinfo_dispose(mi);
            monapi_cmemoryinfo_delete(mi);
            break;
        }

    case COMMAND_CHSH:
        if (monapi_call_elf_execute_file("/SERVERS/1LINESH.SVR", 1) != 0) break;
        for (MessageInfo msg;;)
        {
            if (Message::receive(&msg) != 0) continue;
            if (msg.header == MSG_SERVER_START_OK) break;
        }
        hasExited = true;
        break;

    case COMMAND_UNAME:
        {
            char ver[128];
            syscall_get_kernel_version(ver, 128);
            ver[127] = '\0';
            printf("%s\n", ver);
            break;
        }

    case COMMAND_ECHO:
        for (int i = 1; i < args.get_Length(); i++)
        {
            if (i > 1) printf(" ");
            printf("%s", (const char*)args[i]);
        }
        printf("\n");
        break;

    case COMMAND_CLEAR:
        screen.fillRect16(0, 0, screen.getWidth(), screen.getHeight(), BACKGROUND);
        syscall_set_cursor(0, 0);
        ret = false;
        break;

    case COMMAND_PS:
        {
            syscall_set_ps_dump();
            PsInfo info;

            printf("[tid] [state]  [eip]    [esp]    [cr3]    [name]\n");

            char buf[256];
            while (syscall_read_ps_dump(&info) == 0)
            {
                sprintf(buf, "%05d %s %08x %08x %08x %s\n",
                    info.tid, info.state ? "running" : "waiting",
                    info.eip, info.esp, info.cr3, info.name);
                printf(buf);
            }

            break;
        }

    case COMMAND_KILL:
        {
            if (args.get_Length() < 2)
            {
                printf("usage: KILL tid\n");
                break;
            }

            if (syscall_kill_thread(atoi(args[1])))
            {
                printf("kill failed. Thread not found\n");
            }
            else
            {
                printf("thread %d killed", atoi(args[1]));
            }

            break;
        }

    default:
        break;
    }

    monapi_call_mouse_set_cursor(1);
    return ret;
}
