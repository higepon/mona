#ifdef MONA

#include <monapi.h>
#include <servers/screen.h>
#include "mona_shell.h"
#include "scheme.h"

using namespace MonAPI;

static TerminalUtil* terminal;

int mona_shell_write(const char* format, ...)
{
    char str[512];
    str[0] = '\0';
    va_list args;
    int result;
    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > 512)
    {
        _printf("Shell::out:overflow");
    }
    return terminal->write(str);
}

int mona_shell_init(bool interactive)
{
    if (interactive == MONA_SHELL_INTERCTIVE)
    {
        if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1))
        {
            printf("register to keyboard server failed\n");
            return -1;
        }
    }

    if (!monapi_register_to_server(ID_PROCESS_SERVER, 1))
    {
        printf("register to process server failed\n");
        return -1;
    }


    MessageInfo msg;
    uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
    if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
        printf("SCREEN.EX5 not found\n");
        return -1;
    }
    uint32_t screenHandle = msg.arg2;
    terminal = new TerminalUtil(Stream::FromHandle(screenHandle));
    return 0;

}

int mona_shell_fini()
{
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    monapi_register_to_server(ID_PROCESS_SERVER, 0);

    return 0;
}

static util::String line = "";
static uint32_t cursorPosition = 0;

void mona_shell_init_variables()
{
    line = "";
    cursorPosition = 0;
}

void mona_shell_reedit()
{
    mona_shell_write("\n");
    line += '\n';
    cursorPosition = 0;
    line = "";
}

void mona_shell_back_space()
{
    SCM_TRACE_OUT("");
    SCM_TRACE_OUT("cursorPosition = %d size=%d", cursorPosition, line.size());
    if (cursorPosition == 0) return;
    if (cursorPosition == line.size())
    {
        line.chop();
        SCM_TRACE_OUT("line = [%s]", line.data());
        terminal->eraseCursor();
        terminal->cursorLeft(1);
        terminal->write(" ");
        terminal->cursorLeft(1);
        terminal->drawCursor();
        cursorPosition--;
    }
    else
    {
        terminal->eraseCursor();
        cursorPosition--;
        terminal->cursorLeft(1);
        // remove end of line. Don't move cursorPosition!
        terminal->cursorRight(line.size() - cursorPosition - 1);
        terminal->write(" ");
        terminal->cursorLeft(line.size() - cursorPosition);
        // ^ remove end of line
        line.removeAt(cursorPosition);
        terminal->cursorLeft(cursorPosition);
        terminal->write(line.data());
        terminal->cursorLeft(1);
        terminal->drawCursor();
    }
}

void mona_shell_cursor_backward(int n /* = 1 */)
{
    SCM_TRACE_OUT("");
    if (cursorPosition == 0) return;
    if ((int)cursorPosition < n)
    {
        n = cursorPosition;
    }
    cursorPosition -= n;
    terminal->eraseCursor();
    terminal->cursorLeft(n);
    terminal->drawCursor();
}

void mona_shell_cursor_forward(int n /* = 1 */)
{
    SCM_TRACE_OUT("");
    if (cursorPosition == line.size()) return;
    if (cursorPosition + n > line.size())
    {
        n = line.size() - cursorPosition;
    }
    cursorPosition += n;
    terminal->eraseCursor();
    terminal->cursorRight(n);
    terminal->drawCursor();
}

void mona_shell_del()
{
    SCM_TRACE_OUT("");
    if (line.size() == cursorPosition) return;
    mona_shell_cursor_forward();
    mona_shell_back_space();
}

void mona_shell_cursor_beginning_of_line()
{
    SCM_TRACE_OUT("");
    terminal->eraseCursor();
    terminal->cursorLeft(cursorPosition);
    cursorPosition = 0;
    terminal->drawCursor();
}

void mona_shell_cursor_end_of_line()
{
    SCM_TRACE_OUT("");
    terminal->eraseCursor();
    terminal->cursorRight(line.size() - cursorPosition);
    cursorPosition = line.size();
    terminal->drawCursor();
}

void mona_shell_kill_line()
{
    SCM_TRACE_OUT("");
    uint32_t times = line.size() - cursorPosition;
    mona_shell_cursor_end_of_line();
    for (uint32_t i = 0; i < times; i++)
    {
        mona_shell_back_space();
    }
}

void mona_shell_output(int keycode, int modifiers)
{
    KeyInfo key;
    key.keycode = keycode;
    key.modifiers = modifiers;
    char c = Keys::ToChar(key);
    mona_shell_write("%c", c);
    terminal->drawCursor();
    if (cursorPosition < line.size())
    {
        line.set(cursorPosition, c);
    }
    else
    {
        line += c;
    }
    cursorPosition++;
}

void mona_shell_on_key_down(int keycode, int modifiers)
{

    switch(keycode) {
    case (Keys::H):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_back_space();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::P):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            // history
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::B):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_cursor_backward();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::A):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_cursor_beginning_of_line();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::E):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_cursor_end_of_line();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }

    case (Keys::F):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_cursor_forward();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::K):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_kill_line();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::D):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_del();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }
    case (Keys::C):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            scheme_on_reedit();
            break;
        }
        else
        {
            mona_shell_output(keycode, modifiers);
            break;
        }

    case(Keys::G):
    case(Keys::I):case(Keys::J):case(Keys::L):
    case(Keys::M):case(Keys::N):case(Keys::O):
    case(Keys::Q):case(Keys::R):case(Keys::S):case(Keys::T):
    case(Keys::U):case(Keys::V):case(Keys::W):case(Keys::X):
    case(Keys::Y):case(Keys::Z):case(Keys::Decimal):case(Keys::D0):
    case(Keys::D1):case(Keys::D2):case(Keys::D3):case(Keys::D4):
    case(Keys::D5):case(Keys::D6):case(Keys::D7):case(Keys::D8):
    case(Keys::D9):case(Keys::NumPad1):case(Keys::NumPad2):case(Keys::NumPad3):
    case(Keys::NumPad4):case(Keys::NumPad5):case(Keys::NumPad6):case(Keys::NumPad7):
    case(Keys::NumPad8):case(Keys::NumPad9):case(Keys::NumPad0):case(Keys::Subtract):
    case(Keys::Add):case(Keys::Space):case(Keys::Divide):case(Keys::OemPeriod):
    case(Keys::OemPipe):case(Keys::OemQuestion):case(Keys::OemMinus):case(Keys::OemBackslash):
    case(Keys::OemSemicolon):case(Keys::Oemplus):
        mona_shell_output(keycode, modifiers);
        break;
    case(Keys::Enter):
        mona_shell_write("\n");
        line += '\n';
        scheme_on_input_line(line);
        cursorPosition = 0;
        line = "";
        break;

    case(Keys::Back):
        mona_shell_back_space();
        break;
    default:
        break;
    }
}


#else
int mona_shell_init(bool interactive)
{
}

int mona_shell_fini()
{
}

#endif
