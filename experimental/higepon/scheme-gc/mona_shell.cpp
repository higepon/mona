#ifdef MONA

#include <monapi.h>
#include <monapi/terminal/Util.h>
#include <servers/screen.h>
#include "mona_shell.h"
#include "scheme.h"

using namespace MonAPI;

static terminal::Util* terminal_;

class CommandHistory
{
public:
    CommandHistory() {}
    virtual ~CommandHistory() {}

    void add(::util::String history)
    {
        if (history.size() == 0) return;
        if (history.last() == '\n')
        {
            history.chop();
        }

        histories_.add(new ::util::String(history));
        positionToNewest();
    }

    ::util::String* getOlderHistory()
    {
        positionToOlder();
        return get();
    }

    ::util::String* getNewerHistory()
    {
        positionToNewer();
        return get();
    }

private:

    ::util::String* get()
    {
        if (position_ < 0 || position_ >= histories_.size()) return new ::util::String("");
        return histories_.get(position_);
    }

    void positionToNewest()
    {
        position_ = histories_.size();
    }

    void positionToOlder()
    {
        if (position_ < 0) return;
        position_--;
    }

    void positionToNewer()
    {
        if (position_ >= histories_.size()) return;
        position_++;
    }

private:
    ::util::Vector< ::util::String* > histories_;
    int position_;
};

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
    return terminal_->write(str);
}

static CommandHistory histories;

int mona_shell_init(bool interactive)
{
    if (interactive == MONA_SHELL_INTERCTIVE)
    {
        if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1))
        {
            printf("register to keyboard server failed\n");
            return -1;
        }
        histories.add("(ls)");
        histories.add("(pwd)");
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
    terminal_ = new terminal::Util(Stream::FromHandle(screenHandle));
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

void mona_shell_add_history(::util::String command)
{
    histories.add(command);
}

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
    if (cursorPosition == 0) return;
    if (cursorPosition == line.size())
    {
        line.chop();
        terminal_->eraseCursor();
        terminal_->moveCursorLeft(1);
        terminal_->write(" ");
        terminal_->moveCursorLeft(1);
        terminal_->drawCursor();
        cursorPosition--;
    }
    else
    {
        terminal_->eraseCursor();
        cursorPosition--;
        terminal_->moveCursorLeft(1);
        // remove end of line. Don't move cursorPosition!
        terminal_->moveCursorRight(line.size() - cursorPosition - 1);
        terminal_->write(" ");
        terminal_->moveCursorLeft(line.size() - cursorPosition);
        // ^ remove end of line
        line.removeAt(cursorPosition);
        terminal_->moveCursorLeft(cursorPosition);
        terminal_->write(line.data());
        terminal_->moveCursorLeft(1);
        terminal_->drawCursor();
    }
}

void mona_shell_cursor_backward(int n /* = 1 */)
{
    if (cursorPosition == 0) return;
    if ((int)cursorPosition < n)
    {
        n = cursorPosition;
    }
    cursorPosition -= n;
    terminal_->eraseCursor();
    terminal_->moveCursorLeft(n);
    terminal_->drawCursor();
}

void mona_shell_cursor_forward(int n /* = 1 */)
{
    if (cursorPosition == line.size()) return;
    if (cursorPosition + n > line.size())
    {
        n = line.size() - cursorPosition;
    }
    cursorPosition += n;
    terminal_->eraseCursor();
    terminal_->moveCursorRight(n);
    terminal_->drawCursor();
}

void mona_shell_del()
{
    if (line.size() == cursorPosition) return;
    mona_shell_cursor_forward();
    mona_shell_back_space();
}

void mona_shell_cursor_beginning_of_line()
{
    terminal_->eraseCursor();
    terminal_->moveCursorLeft(cursorPosition);
    cursorPosition = 0;
    terminal_->drawCursor();
}

void mona_shell_cursor_end_of_line()
{
    terminal_->eraseCursor();
    terminal_->moveCursorRight(line.size() - cursorPosition);
    cursorPosition = line.size();
    terminal_->drawCursor();
}

void mona_shell_kill_line()
{
    uint32_t times = line.size() - cursorPosition;
    mona_shell_cursor_end_of_line();
    for (uint32_t i = 0; i < times; i++)
    {
        mona_shell_back_space();
    }
}

void mona_shell_output_line(::util::String l)
{
    mona_shell_cursor_beginning_of_line();
    mona_shell_kill_line();
    line = l;
    mona_shell_write("%s", line.data());
    cursorPosition = line.size();
}

void mona_shell_output_char(char c)
{
    line.insert(cursorPosition, c);
    mona_shell_write("%c", c);

    if (cursorPosition != line.size() - 1)
    {
        mona_shell_write("%s", line.substring(cursorPosition + 1, line.size() - cursorPosition).data());
        terminal_->moveCursorLeft(line.size() - cursorPosition - 1);
    }
    terminal_->drawCursor();
    cursorPosition++;
}

void mona_shell_output_key(int keycode, int modifiers)
{
    KeyInfo key;
    key.keycode = keycode;
    key.modifiers = modifiers;
    mona_shell_output_char(Keys::ToChar(key));
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
            mona_shell_output_key(keycode, modifiers);
            break;
        }
    case(Keys::Up):
        mona_shell_output_line(histories.getOlderHistory()->data());
        break;
    case(Keys::Down):
        mona_shell_output_line(histories.getNewerHistory()->data());
        break;

    case (Keys::P):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_output_line(histories.getOlderHistory()->data());
            break;
        }
        else
        {
            mona_shell_output_key(keycode, modifiers);
            break;
        }
    case (Keys::N):
        if (modifiers & KEY_MODIFIER_CTRL)
        {
            mona_shell_output_line(histories.getNewerHistory()->data());
            break;
        }
        else
        {
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
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
            mona_shell_output_key(keycode, modifiers);
            break;
        }

    case(Keys::G):
    case(Keys::I):case(Keys::J):case(Keys::L):
    case(Keys::M):case(Keys::O):
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
        mona_shell_output_key(keycode, modifiers);
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
    return 0;
}

int mona_shell_fini()
{
    return 0;
}

void mona_shell_reedit()
{
}

#include "util/String.h"
void mona_shell_add_history(::util::String command)
{
}
#endif
