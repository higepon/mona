#include <monapi/messages.h>
#include <monapi/Keys.h>
#include <monapi.h>
#include "Shell.h"


/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell(bool callAutoExec)
    : position(0), hasExited(false), callAutoExec(callAutoExec),
      waiting(THREAD_UNKNOWN), prevX(0), prevY(0)
{
    this->current = STARTDIR;
    this->makeApplicationList();

    if (this->callAutoExec)
    {
        this->executeMSH("/AUTOEXEC.MSH");
        this->callAutoExec = false;
    }

    this->self = syscall_get_tid();

    this->printPrompt("\n");
    this->drawCaret();
}

Shell::~Shell()
{
}

void Shell::run()
{
    MessageInfo msg;
    while (!this->hasExited)
    {
        if (MonAPI::Message::receive(&msg) != 0) continue;

#if 0  /// DEBUG for message
        if ((msg.header == MSG_RESULT_OK && msg.arg1 == MSG_PROCESS_STDOUT_DATA) || msg.header == MSG_PROCESS_STDOUT_DATA)
        {
            char buf[128];
            sprintf(buf, "**** INVALID MESSAGE!! ****[%d: %d, %d]\n", syscall_get_tid(), msg.header, msg.arg1);
            syscall_print(buf);
            for (;;);
        }
#endif
        switch (msg.header)
        {
            case MSG_KEY_VIRTUAL_CODE:
                if (msg.arg2 & KEY_MODIFIER_DOWN)
                {
                    this->onKeyDown(msg.arg1, msg.arg2);
                }
                break;
            case MSG_PROCESS_TERMINATED:
                if (this->waiting == msg.arg1)
                {
                    this->printPrompt("\n");
                    this->drawCaret();
                    this->waiting = THREAD_UNKNOWN;
                }

                break;
            case MSG_STDOUT:

                msg.str[127] = '\0';
                syscall_print(msg.str);
                MonAPI::Message::reply(&msg);
                break;
        }
    }
}

void Shell::backspace()
{
    if (this->position == 0)
    {
        /* donothing */
        return;
    }

    this->checkCaretPosition();

    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - 1, y);
    printf("  ");
    syscall_set_cursor(x - 1, y);
    this->drawCaret();

    /* backspace */
    this->position--;
}

void Shell::commandChar(char c)
{
    if (c != '\0')
    {
        this->checkCaretPosition();
        int x, y;
        syscall_get_cursor(&x, &y);
        x++;
        if ((x + 1) * FONT_WIDTH >= this->screen.getWidth()) return;

        printf("%c", c);
        this->drawCaret();
    }
    this->commandLine[this->position] = c;
    this->position++;
}

void Shell::commandExecute(bool prompt)
{
    if (prompt) printf("\n");

    System::Array<MonAPI::CString> args = this->parseCommandLine();
    this->position = 0;
    if (args.get_Length() == 0)
    {
        /* command is empty */
        this->printPrompt();
        return;
    }

    putHistory(this->commandLine);

    /* internal command */
    int isInternal = isInternalCommand(args[0]);
    if (isInternal != 0)
    {
        bool newline = internalCommandExecute(isInternal, args);
        if (!this->hasExited && prompt)
        {
            this->printPrompt(newline ? "\n" : NULL);
        }
        return;
    }

    MonAPI::CString cmdLine;
    MonAPI::CString command = args[0].toUpper();
    if (command[0] == '/')
    {
        cmdLine = command;
    }
    else if (command.endsWith(".BIN") || command.endsWith(".BN2")
        || command.endsWith(".ELF") || command.endsWith(".EL2")
        || command.endsWith(".EXE") || command.endsWith(".EX2"))
    {
        cmdLine = APPSDIR"/" + command;
    }
    else if (command.endsWith(".APP"))
    {
        MonAPI::CString name = command.substring(0, command.getLength() - 4);
        cmdLine = APPSDIR"/" + name + ".APP/" + name + ".EL2";
    }
    else
    {
        MonAPI::CString cmd2 = command + ".";
        for (int i = 0; i < this->apps.size(); i++)
        {
            MonAPI::CString file = apps.get(i);
            if (file == command + ".APP")
            {
                cmdLine = APPSDIR"/" + file + "/" + command + ".EL2";
                break;
            }
            else if (file.startsWith(cmd2))
            {
                cmdLine = APPSDIR"/" + file;
                break;
            }
        }
        if (cmdLine == NULL)
        {
            printf("Can not find command.\n");
            if (this->waiting == THREAD_UNKNOWN) this->printPrompt("\n");
            return;
        }
    }

    if (cmdLine.endsWith(".MSH"))
    {
        this->executeMSH(cmdLine);
        if (this->waiting == THREAD_UNKNOWN) this->printPrompt("\n");
        return;
    }

    for (int i = 1; i < args.get_Length(); i++)
    {
        cmdLine += ' ';
        cmdLine += args[i];
    }

    dword tid;
    int result = monapi_call_process_execute_file_get_tid(cmdLine, MONAPI_TRUE, &tid, this->self);

    if (!this->callAutoExec && result == 0)
    {
        this->waiting = tid;
    }
    else
    {
        this->printPrompt("\n");
    }
}

void Shell::commandTerminate()
{
    commandChar('\0');
}

void Shell::putHistory(const MonAPI::CString& command)
{
    history.add(command);
}

MonAPI::CString Shell::getHistory()
{
    if (history.isEmpty()) return "";
    return history.get(0);
}

void Shell::onKeyDown(int keycode, int modifiers)
{
    if (this->waiting != THREAD_UNKNOWN)
    {
        this->printPrompt("\n");
        this->waiting = THREAD_UNKNOWN;
        if (keycode == MonAPI::Keys::Enter) return;
    }

    switch(keycode) {
    case(MonAPI::Keys::A):
    case(MonAPI::Keys::B):
    case(MonAPI::Keys::C):
    case(MonAPI::Keys::D):
    case(MonAPI::Keys::E):
    case(MonAPI::Keys::F):
    case(MonAPI::Keys::G):
    case(MonAPI::Keys::H):
    case(MonAPI::Keys::I):
    case(MonAPI::Keys::J):
    case(MonAPI::Keys::K):
    case(MonAPI::Keys::L):
    case(MonAPI::Keys::M):
    case(MonAPI::Keys::N):
    case(MonAPI::Keys::O):
    case(MonAPI::Keys::P):
    case(MonAPI::Keys::Q):
    case(MonAPI::Keys::R):
    case(MonAPI::Keys::S):
    case(MonAPI::Keys::T):
    case(MonAPI::Keys::U):
    case(MonAPI::Keys::V):
    case(MonAPI::Keys::W):
    case(MonAPI::Keys::X):
    case(MonAPI::Keys::Y):
    case(MonAPI::Keys::Z):
    case(MonAPI::Keys::Decimal):
    case(MonAPI::Keys::D0):
    case(MonAPI::Keys::D1):
    case(MonAPI::Keys::D2):
    case(MonAPI::Keys::D3):
    case(MonAPI::Keys::D4):
    case(MonAPI::Keys::D5):
    case(MonAPI::Keys::D6):
    case(MonAPI::Keys::D7):
    case(MonAPI::Keys::D8):
    case(MonAPI::Keys::D9):
    case(MonAPI::Keys::NumPad1):
    case(MonAPI::Keys::NumPad2):
    case(MonAPI::Keys::NumPad3):
    case(MonAPI::Keys::NumPad4):
    case(MonAPI::Keys::NumPad5):
    case(MonAPI::Keys::NumPad6):
    case(MonAPI::Keys::NumPad7):
    case(MonAPI::Keys::NumPad8):
    case(MonAPI::Keys::NumPad9):
    case(MonAPI::Keys::NumPad0):
    case(MonAPI::Keys::Subtract):
    case(MonAPI::Keys::Add):
    case(MonAPI::Keys::Space):
    case(MonAPI::Keys::Divide):
    case(MonAPI::Keys::OemPeriod):
    case(MonAPI::Keys::OemQuestion):
        KeyInfo key;
        key.keycode = keycode;
        key.modifiers = modifiers;
        this->commandChar(MonAPI::Keys::ToChar(key));
        break;
    case(MonAPI::Keys::Enter):
        this->drawCaret(true);
        this->commandTerminate();
        this->commandExecute(true);
        if (this->waiting == THREAD_UNKNOWN && !this->hasExited) this->drawCaret();
        break;

    case(MonAPI::Keys::Up):
        printf("up");
        break;
    case(MonAPI::Keys::Down):
        printf("down");
        break;
    case(MonAPI::Keys::Left):
        printf("left");
        break;
    case(MonAPI::Keys::Right):
        printf("right");
        break;
    case(MonAPI::Keys::Back):
        backspace();
        break;
    default:
        break;
    }
}

System::Array<MonAPI::CString> Shell::parseCommandLine()
{
    System::Array<MonAPI::CString> args = MonAPI::CString(this->commandLine).split(' ');
    int size = 0;
    FOREACH(MonAPI::CString, arg, args)
    {
        if (arg != NULL) size++;
    }
    END_FOREACH

    System::Array<MonAPI::CString> ret(size);
    int i = 0;
    FOREACH(MonAPI::CString, arg, args)
    {
        if (arg == NULL) continue;

        ret[i++] = arg;
    }
    END_FOREACH

    return ret;
}

int Shell::makeApplicationList()
{
    char name[15];
    int  size;
    int  attr;

    if (syscall_cd(APPSDIR) != 0)
    {
        printf("%s: application list error\n", SVR);
        return 1;
    }

    if (syscall_dir_open() != 0)
    {
        printf("%s: application dir open error\n", SVR);
        return 2;
    }

    while (syscall_dir_read(name, &size, &attr) == 0)
    {
        MonAPI::CString file = name;
        if (file.endsWith(".BIN") || file.endsWith(".BN2")
            || file.endsWith(".ELF") || file.endsWith(".EL2")
            || file.endsWith(".EXE") || file.endsWith(".EX2")
            || file.endsWith(".APP") || file.endsWith(".MSH"))
        {
            apps.add(name);
        }
    }

    syscall_dir_close();

    if (APPSDIR != this->current)
    {
        if (syscall_cd(this->current) != 0)
        {
            this->current = "/";
            syscall_cd(this->current);
        }
    }

    return 0;
}

void Shell::printPrompt(const MonAPI::CString& prefix /*= NULL*/)
{
    if (prefix != NULL) printf("%s", (const char*)prefix);
    printf("[Mona]%s> ", (const char*)this->current);
}

MonAPI::CString Shell::getParentDirectory(const MonAPI::CString& dir)
{
    if (dir == NULL || dir == "/") return "/";

    int p = dir.lastIndexOf('/');
    if (p < 1) return "/";

    return dir.substring(0, p);
}

MonAPI::CString Shell::mergeDirectory(const MonAPI::CString& dir1, const MonAPI::CString& dir2)
{
    if (dir2.startsWith("/")) return dir2.toUpper();

    MonAPI::CString ret = dir1;
    System::Array<MonAPI::CString> dirs = dir2.split('/');
    FOREACH(MonAPI::CString, d, dirs)
    {
        if (d == NULL || d == ".") continue;

        if (d == "..")
        {
            ret = this->getParentDirectory(ret);
        }
        else
        {
            if (ret != "/") ret += '/';
            ret += d.toUpper();
        }
    }
    END_FOREACH
    return ret;
}

void Shell::printFiles(const MonAPI::CString& dir)
{
    char name[15];
    int size, attr;

    if (syscall_cd(dir) != 0)
    {
        printf("%s: directory not found: %s\n", SVR, (const char*)dir);
        return;
    }
    if (syscall_dir_open())
    {
        printf("%s: can not open directory: %s\n", SVR, (const char*)dir);
        return;
    }

    MonAPI::CString spc = "               ";
    int w = 0, sw = this->screen.getWidth();
    while (syscall_dir_read(name, &size, &attr) == 0)
    {
        MonAPI::CString file = name;
        if ((attr & ATTRIBUTE_DIRECTORY) != 0)
        {
            file = "[" + file + "]";
        }

        file = (file + spc).substring(0, 15);
        int fw = FONT_WIDTH * file.getLength();
        if (w + fw >= sw)
        {
            printf("\n");
            w = 0;
        }
        printf("%s", (const char*)file);
        w += fw;
    }
    printf("\n");

    syscall_dir_close();
    syscall_cd(this->current);
}

void Shell::executeMSH(const MonAPI::CString& msh)
{
    monapi_cmemoryinfo* mi = monapi_call_file_read_data(msh, 1);
    if (mi == NULL) return;

    for (dword pos = 0, start = 0; pos <= mi->Size; pos++)
    {
        char ch = pos < mi->Size ? (char)mi->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            int len = pos - start;
            bool prompt = true;
            if (len > 0)
            {
                if (mi->Data[start] == '@')
                {
                    start++;
                    len--;
                    prompt = false;
                }
            }
            if (len > 0)
            {
                if (len > 127) len = 127;
                strncpy(this->commandLine, (const char*)&mi->Data[start], len);
                this->position = len;
                this->commandLine[len] = '\0';
                if (prompt)
                {
                    this->printPrompt("\n");
                    printf("%s", this->commandLine);
                }
                this->commandExecute(prompt);
            }
            start = pos + 1;
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void Shell::drawCaret(bool erase /*= false*/)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    this->screen.fillRect16(x * FONT_WIDTH, y * FONT_HEIGHT + FONT_HEIGHT - 2, 8, 2,
        erase ? BACKGROUND : FOREGROUND);
    this->prevX = x;
    this->prevY = y;
}

void Shell::checkCaretPosition()
{
    int x, y;
    syscall_get_cursor(&x, &y);
    if (this->prevX == x && this->prevY == y) return;

    monapi_call_mouse_set_cursor(0);
    this->printPrompt(this->prevX == 0 ? NULL : "\n");
    this->commandLine[this->position] = '\0';
    printf(this->commandLine);
    monapi_call_mouse_set_cursor(1);
}
