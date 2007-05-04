#include <monapi/messages.h>
#include <monapi/Keys.h>
#include <monalibc/stdarg.h>
#include <monalibc/stdio.h>

#include "Shell.h"
using namespace MonAPI;

/*----------------------------------------------------------------------
    Shell
----------------------------------------------------------------------*/
Shell::Shell(uint32_t outHandle) : position_(0), doExec_(false), waiting_(THREAD_UNKNOWN)
{
    inStream_ = new Stream();
    outStream_ = Stream::FromHandle(outHandle);
    terminal_ = new terminal::Util(outStream_);
    changeDirecotory(APPSDIR);
    startDirectory_ = APPSDIR;

    executeMSH("/AUTOEXEC.MSH");
    prompt();
    terminal_->drawCursor();
}

Shell::~Shell()
{
    delete inStream_;
    delete outStream_;
}

void Shell::run()
{
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;
        switch (msg.header)
        {
            case MSG_KEY_VIRTUAL_CODE:
                if (!doExec_ && (msg.arg2 & KEY_MODIFIER_DOWN) != 0)

                {
                    this->onKeyDown(msg.arg1, msg.arg2);
                }
                else if (msg.arg1 == 0)
                {
                    this->onKeyDown(msg.arg2, msg.arg3);
                }
                break;

            case MSG_PROCESS_TERMINATED:
                if (waiting_ == msg.arg1)
                {
                    prompt();
                    waiting_ = THREAD_UNKNOWN;
                    doExec_ = false;
                    terminal_->drawCursor();
                }
                break;
            case MSG_GET_OUT_STREAM_HANDLE:
            {
                Message::reply(&msg, outStream_->handle());
                break;
            }
            case MSG_CHANGE_OUT_STREAM_BY_HANDLE:
            {
                uint32_t oldHandle = outStream_->handle();
                // delete outStream_;
                outStream_ = Stream::FromHandle(msg.arg1);
                terminal_ = new terminal::Util(outStream_);
                Message::reply(&msg, oldHandle);
                break;
            }
        }
    }
}

void Shell::backspace()
{
    if (position_ == 0)
    {
        /* donothing */
        return;
    }

    terminal_->moveCursorLeft(1);
    formatWrite("  ");
    terminal_->moveCursorLeft(2);
    terminal_->drawCursor();

    /* backspace */
    position_--;
}

void Shell::commandChar(char c)
{
    if (c != '\0')
    {
        formatWrite("%c", c);
        terminal_->drawCursor();
    }

    // failure of locking means that other process wants shell to write key information and he or she wants to read them from their stdin.
    if (inStream_->tryLockForRead() == MONA_SUCCESS)
    {
        inStream_->unlockForRead();
        commandLine_[position_] = c;
        position_++;
    }
    else
    {
        inStream_->write((uint8_t*)&c, 1);
    }
}

bool Shell::isPipeCommand()
{
    CString line(commandLine_);
    return line.indexOf('|') != -1;
}

void Shell::commandExecutePipe()
{
    _A<CString> commands = CString(commandLine_).split('|');
    for (int i = 0; i < commands.get_Length(); i++)
    {
        commands[i].trim();
    }
    _A< _A<CString> > parsedCommands(commands.get_Length());
    for (int i = 0; i < commands.get_Length(); i++)
    {
        parsedCommands[i] = commands[i].split(' ');
        if (isInternalCommand(parsedCommands[i][0]))
        {
            formatWrite("internal command can not use pipe yet\n");
            prompt(false);
            return;
        }
    }

    _A<Stream*> streams(commands.get_Length() + 1);
    streams[0] = inStream_;
    streams[commands.get_Length()] = outStream_;
    for (int i = 1; i < commands.get_Length(); i++)
    {
        streams[i] = new Stream();
    }

    for (int i = 0; i < commands.get_Length(); i++)
    {
        commandExecute(parsedCommands[i], streams[i]->handle(), streams[i + 1]->handle());
    }
}

void Shell::commandExecute(bool isPrompt)
{
    if (isPrompt) formatWrite("\n");

    if (isPipeCommand())
    {
        commandExecutePipe();
        position_ = 0;
        return;
    }

    _A<CString> args = this->parseCommandLine();
    position_ = 0;
    if (args.get_Length() == 0)
    {
        /* command is empty */
        prompt(false);
        return;
    }

    /* internal command */
    int isInternal = isInternalCommand(args[0]);
    if (isInternal != 0)
    {
        bool newline = internalCommandExecute(isInternal, args);
        if (isPrompt)
        {
            prompt(newline);
        }
        return;
    }

    this->commandExecute(args, inStream_->handle(), outStream_->handle());
}

bool Shell::commandExecute(_A<CString> args, uint32_t stdin_id, uint32_t stdout_id)
{
    history_.add(commandLine_);
    CString cmdLine;
    CString command = args[0].toUpper();
    if (command[0] == '/')
    {
        cmdLine = command;
    }
    else if (command.endsWith(".BIN") || command.endsWith(".BN2") || command.endsWith(".BN5")
        || command.endsWith(".ELF") || command.endsWith(".EL2") || command.endsWith(".EL5")
        || command.endsWith(".EXE") || command.endsWith(".EX2") || command.endsWith(".EX5"))
    {
        cmdLine = APPSDIR"/" + command;
    }
    else if (command.endsWith(".APP"))
    {
        CString name = command.substring(0, command.getLength() - 4);
        cmdLine = APPSDIR"/" + name + ".APP/" + name + ".EX5";
    }
    else
    {
        CString cmd2 = command + ".";
        for (int i = 0; i < apps_.size(); i++)
        {
            CString file = apps_.get(i);
            if (file == command + ".APP")
            {
                cmdLine = APPSDIR"/" + file + "/" + command + ".EX5";
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
            formatWrite("Can not find command.\n");
            if (waiting_ == THREAD_UNKNOWN) prompt();
            return false;;
        }
    }

    if (cmdLine.endsWith(".MSH"))
    {
        this->executeMSH(cmdLine);
        if (waiting_ == THREAD_UNKNOWN) prompt();
        return true;
    }

    for (int i = 1; i < args.get_Length(); i++)
    {
        cmdLine += ' ';
        cmdLine += args[i];
    }

    uint32_t tid;
    int result = monapi_call_process_execute_file_get_tid(cmdLine, MONAPI_TRUE, &tid, stdin_id, stdout_id);

    if (result == 0 && !doExec_)
    {
        waiting_ = tid;
    }
    else
    {
        prompt();
    }

    return result == 0;
}

void Shell::commandTerminate()
{
    commandChar('\0');
}

void Shell::onKeyDown(int keycode, int modifiers)
{
    if (!doExec_ && waiting_ != THREAD_UNKNOWN)
    {
        prompt();
        waiting_ = THREAD_UNKNOWN;
        if (keycode == Keys::Enter) return;
    }

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
    case(Keys::OemPipe):
    case(Keys::OemQuestion):
    case(Keys::OemMinus):
    case(Keys::OemBackslash):
    case(Keys::OemSemicolon):
    case(Keys::Oemplus):
        if (keycode == Keys::H && modifiers & KEY_MODIFIER_CTRL)
        {
            backspace();
            break;
        }
        else if (keycode == Keys::P && modifiers & KEY_MODIFIER_CTRL)
        {
            if (history_.size() > 0)
            {
                CString command = history_.get(history_.size() - 1);
                for (int i = 0; i < command.getLength(); i++)
                {
                    this->commandChar(command[i]);
                }
            }
            break;
        }

        KeyInfo key;
        key.keycode = keycode;
        key.modifiers = modifiers;
        this->commandChar(Keys::ToChar(key));
        break;
    case(Keys::Enter):
        terminal_->eraseCursor();
        commandTerminate();
        commandTerminate();
        commandExecute(true);
        if (waiting_ == THREAD_UNKNOWN) terminal_->drawCursor();
        break;

    case(Keys::Up):

        break;
    case(Keys::Down):

        break;
    case(Keys::Left):

        break;
    case(Keys::Right):

        break;
    case(Keys::Back):
        backspace();
        break;
    default:
        break;
    }
}

_A<CString> Shell::parseCommandLine()
{
    _A<CString> args = CString(commandLine_).split(' ');
    int size = 0;
    FOREACH (CString, arg, args)
    {
        if (arg != NULL) size++;
    }
    END_FOREACH

    _A<CString> ret(size);
    int i = 0;
    FOREACH (CString, arg, args)
    {
        if (arg == NULL) continue;

        ret[i++] = arg;
    }
    END_FOREACH

    return ret;
}

int Shell::makeApplicationList()
{
    if (apps_.size() > 0) return 0;
    monapi_cmemoryinfo* mi = monapi_file_read_directory(APPSDIR);
    int size = *(int*)mi->Data;
    if (mi == NULL || size == 0)
    {
        return 1;
    }

    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    for (int i = 0; i < size; i++, p++)
    {
        CString file = p->name;
        if (file.endsWith(".BIN") || file.endsWith(".BN2") || file.endsWith(".BN5")
            || file.endsWith(".ELF") || file.endsWith(".EL2") || file.endsWith(".EL5")
            || file.endsWith(".EXE") || file.endsWith(".EX2") || file.endsWith(".EX5")
            || file.endsWith(".APP") || file.endsWith(".MSH"))
        {
            apps_.add(file);
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
    return 0;
}

CString Shell::getParentDirectory(const CString& dir)
{
    if (dir == NULL || dir == "/") return "/";

    int p = dir.lastIndexOf('/');
    if (p < 1) return "/";

    return dir.substring(0, p);
}

CString Shell::mergeDirectory(const CString& dir1, const CString& dir2)
{
    if (dir2.startsWith("/")) return dir2.toUpper();

    CString ret = dir1;
    _A<CString> dirs = dir2.split('/');
    FOREACH (CString, d, dirs)
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

void Shell::printFiles(const CString& dir)
{
    monapi_cmemoryinfo* mi = monapi_file_read_directory(dir);
    int size;
    if (mi == NULL || (size = *(int*)mi->Data) == 0)
    {
        formatWrite("%s: directory not found: %s\n", SVR, (const char*)dir);
        return;
    }

    CString spc = "               ";
    int w = 0, sw = screen_.getWidth();

    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    for (int i = 0; i < size; i++, p++)
    {
        CString file = p->name;

        if ((p->attr & ATTRIBUTE_DIRECTORY) != 0)
        {
            file = "[" + file + "]";
        }

        file = (file + spc).substring(0, 15);
        int fw = FONT_WIDTH * file.getLength();
        if (w + fw >= sw)
        {
            formatWrite("\n");
            w = 0;
        }
        formatWrite("%s", (const char*)file);
        w += fw;
    }
    formatWrite("\n");

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void Shell::executeMSH(const CString& msh)
{
    monapi_cmemoryinfo* mi = monapi_file_read_all(msh);
    if (mi == NULL) return;

    for (uint32_t pos = 0, start = 0; pos <= mi->Size; pos++)
    {
        char ch = pos < mi->Size ? (char)mi->Data[pos] : '\n';
        if (ch == '\r' || ch == '\n')
        {
            int len = pos - start;
            bool isPrompt = true;
            if (len > 0)
            {
                if (mi->Data[start] == '@')
                {
                    start++;
                    len--;
                    isPrompt = false;
                }
                if (mi->Data[start] == '#')
                {
                    len = 0;
                }
            }
            if (len > 0)
            {
                if (len > 127) len = 127;
                strncpy(commandLine_, (const char*)&mi->Data[start], len);
                position_ = len;
                commandLine_[len] = '\0';
                if (isPrompt)
                {
                    prompt();
                    formatWrite("%s", commandLine_);
                }

                this->commandExecute(isPrompt);
            }
            start = pos + 1;
        }
    }

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

bool Shell::changeDirecotory(const MonAPI::CString& path)
{
    currentDirectory_ = path;
    makeApplicationList();
    return true;
}

int Shell::prompt(bool newline /* = true */)
{
    return formatWrite("%s[Mona]%s> ", newline ? "\n" : "", (const char*)currentDirectory_);
}

int Shell::formatWrite(const char* format, ...)
{
    char str[512];
    str[0] = '\0';
    va_list args;
    int result;
    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > OUT_BUFFER_SIZE)
    {
        printf("Shell::out:overflow");
    }
    return terminal_->write(str);
}
