#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Keys.h>

#include "Shell.h"
#include "elf.h"

#define PROMPT "Mona>"

using namespace MonAPI;

static bool hasExited = false;
static bool callAutoExec = true;

int MonaMain(List<char*>* pekoe)
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1)) exit(1);

    /* Server start ok */
    dword targetID = Message::lookupMainThread("INIT");
    if (targetID == THREAD_UNKNOWN)
    {
        targetID = Message::lookupMainThread("1LINESH.SVR");
        if (targetID == THREAD_UNKNOWN)
        {
            printf("ShellServer:INIT not found\n");
            exit(1);
        }
        callAutoExec = false;
    }

    /* send */
    if (Message::send(targetID, MSG_SERVER_START_OK))
    {
        printf("ShellServer:INIT error\n");
    }

    /* service loop */
    Shell shell;
    MessageInfo info;
    while (!hasExited)
    {
        if (!Message::receive(&info) && info.arg2 & KEY_MODIFIER_DOWN)
        {
            shell.onKeyDown(info.arg1, info.arg2);
        }
    }

    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    return 0;
}

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell() : position_(0)
{
    if (!callAutoExec) printf("\n");
    printf(PROMPT);
    if (!callAutoExec) return;

    monapi_cmemoryinfo* mi = monapi_call_file_read_data("/AUTOEXEC.MSH", 0);
    if (mi == NULL) return;

    for (dword pos = 0; pos <= mi->Size; pos++)
    {
        char ch = pos < mi->Size ? (char)mi->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            if (position_ > 0)
            {
                commandTerminate();
                commandExecute();
            }
        }
        else
        {
            commandChar(ch);
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

Shell::~Shell()
{
}

void Shell::commandChar(char c)
{
    printf("%c", c);
    commandLine_[position_] = c;
    position_++;
}

enum
{
    COMMAND_NONE,
    COMMAND_LS,
    COMMAND_CD,
    COMMAND_CAT,
    COMMAND_CHSH,
    COMMAND_UNAME,
    COMMAND_HELP
};

int Shell::isInternalCommand(const CString& command)
{
    CString cmd = command.toLower();
    if (cmd == "ls" || cmd == "dir")
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
    else if (cmd == "help" || cmd == "?")
    {
        return COMMAND_HELP;
    }
    return COMMAND_NONE;
}

void Shell::commandExecute()
{
    printf("\n");

    if (position_ < 2)
    {
        /* command is empty */
        printf(PROMPT);
        position_ = 0;
        return;
    }

    putHistory(commandLine_);

    /* list initilize */
    CommandOption list;
    list.next = NULL;

    CommandOption* option = NULL;
    CString command;
    _A<CString> args = CString(commandLine_).split(' ');
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        if (command == NULL) command = arg.toUpper();
        option = new CommandOption;
        strncpy(option->str, arg, sizeof(option->str));
        option->next = list.next;
        list.next = option;
    }

    /* internal command */
    int isInternal;
    if ((isInternal = isInternalCommand(command)))
    {
        internalCommandExecute(isInternal, option);
        for (option = list.next; option; option = option->next)
        {
            delete option;
        }
        if (!hasExited) printf("\n%s", PROMPT);
        position_ = 0;
        return;
    }

    CString path;

    if (command[0] == '/')
    {
        path = command;
        int p = path.lastIndexOf('/') + 1;
        command = path.substring(p, path.getLength() - p);
    }
    else if (command.endsWith(".ELF"))
    {
        path = "/APPS/" + command;
    }
    else if (command.endsWith(".APP"))
    {
        CString name = command.substring(0, command.getLength() - 4);
        command = name + ".ELF";
        path = "/APPS/" + name + ".APP/" + command;
    }
    else
    {
        command += ".ELF";
        path = "/APPS/" + command;
    }

    executeProcess(path, command, &list);

    for (option = list.next; option; option = option->next)
    {
        delete option;
    }

    printf("\n%s", PROMPT);
    position_ = 0;
}

void Shell::internalCommandExecute(int command, CommandOption* option)
{
    switch (command)
    {
    case COMMAND_CD:
        {
            if (option == NULL)
            {
                printf("usage: CD directory\n");
            }
            else if (syscall_cd(option->str))
            {
                printf("directory not found: %s\n", option->str);
            }
            break;
        }
    case COMMAND_LS:
        {
            char name[15];
            int  size;

            if (syscall_dir_open())
            {
                printf("dir open error\n");
                break;
            }

            while (syscall_dir_read(name, &size) == 0)
            {
                printf("%s\n", name);
            }

            syscall_dir_close();
            break;
        }
    case COMMAND_CAT:
        {
            if (option == NULL)
            {
                printf("usage: CAT/TYPE file\n");
                break;
            }

            monapi_cmemoryinfo* mi = monapi_call_file_read_data(option->str, 1);
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
        if (executeProcess("/SERVERS/1LINESH.SVR", "1LINESH.SVR", NULL) != 0) break;
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
    case COMMAND_HELP:
        printf("* Mona Shell Internal Commands\n");
        printf("LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, HELP/?\n");
        break;
    default:
        break;
    }
}

void Shell::commandTerminate() {
    commandChar('\0');
}

void Shell::putHistory(const CString& command)
{
    history.add(command);
}

CString Shell::getHistory()
{
    if (history.isEmpty()) return "";
    return history.get(0);
}

void Shell::backspace() {

    if (position_ == 0) {
        /* donothing */
        return;
    }

    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - 1, y);
    printf(" ");
    syscall_set_cursor(x - 1, y);

    /* backspace */
    position_--;
}

int Shell::onKeyDown(int keycode, int modifiers) {

    switch(keycode) {
    case(Keys::A):
    case(Keys::B):
    case(Keys::C):
    case(Keys::D):
    case(Keys::E):
    case(Keys::F):
    case(Keys::G):
    case(Keys::H):
    case(Keys::I):
    case(Keys::J):
    case(Keys::K):
    case(Keys::L):
    case(Keys::M):
    case(Keys::N):
    case(Keys::O):
    case(Keys::P):
    case(Keys::Q):
    case(Keys::R):
    case(Keys::S):
    case(Keys::T):
    case(Keys::U):
    case(Keys::V):
    case(Keys::W):
    case(Keys::X):
    case(Keys::Y):
    case(Keys::Z):
    case(Keys::Decimal):
    case(Keys::D0):
    case(Keys::D1):
    case(Keys::D2):
    case(Keys::D3):
    case(Keys::D4):
    case(Keys::D5):
    case(Keys::D6):
    case(Keys::D7):
    case(Keys::D8):
    case(Keys::D9):
    case(Keys::NumPad1):
    case(Keys::NumPad2):
    case(Keys::NumPad3):
    case(Keys::NumPad4):
    case(Keys::NumPad5):
    case(Keys::NumPad6):
    case(Keys::NumPad7):
    case(Keys::NumPad8):
    case(Keys::NumPad9):
    case(Keys::NumPad0):
    case(Keys::Subtract):
    case(Keys::Add):
    case(Keys::Space):
    case(Keys::Divide):
    case(Keys::OemPeriod):
    case(Keys::OemQuestion):
        KeyInfo key;
        key.keycode = keycode;
        key.modifiers = modifiers;
        commandChar(Keys::ToChar(key));
        break;
    case(Keys::Enter):
        commandTerminate();
        commandExecute();
        break;

    case(Keys::Up):
        printf("up");
        break;
    case(Keys::Down):
        printf("down");
        break;
    case(Keys::Left):
        printf("left");
        break;
    case(Keys::Right):
        printf("right");
        break;
    case(Keys::Back):
        backspace();
        break;
    default:
        break;

    }
    return 0;
}

int Shell::executeProcess(const CString& path, const CString& name, CommandOption* option)
{
    monapi_cmemoryinfo* mi1 = monapi_call_file_read_data(path, 1);
    if (mi1 == NULL) return -1;

    ELFLoader loader;

    int imageSize = loader.prepare((dword)mi1->Data);
    if (imageSize < 0)
    {
        printf("unknown executable format: %d\n", imageSize);
        monapi_cmemoryinfo_dispose(mi1);
        monapi_cmemoryinfo_delete(mi1);
        return imageSize;
    }

    monapi_cmemoryinfo* mi2 = monapi_cmemoryinfo_new();
    if (!monapi_cmemoryinfo_create(mi2, imageSize, 0))
    {
        printf("image buffer allocate error\n");
        monapi_cmemoryinfo_delete(mi2);
        monapi_cmemoryinfo_dispose(mi1);
        monapi_cmemoryinfo_delete(mi1);
        return -1;
    }

    dword entrypoint = loader.load(mi2->Data);

    monapi_cmemoryinfo_dispose(mi1);
    monapi_cmemoryinfo_delete(mi1);

    LoadProcessInfo info;
    info.image = mi2->Data;
    info.size = imageSize;
    info.entrypoint = entrypoint;
    info.path = path;
    info.name = name;
    info.list = option;

    int result = syscall_load_process_image(&info);

    monapi_cmemoryinfo_dispose(mi2);
    monapi_cmemoryinfo_delete(mi2);

    switch(result)
    {
      case(0):
          break;
      case(4):
          printf("Shared Memory error1");
          break;
      case(5):
          printf("Shared Memory error2");
          break;
      default:
          break;
    }

    return result;
}
