#ifdef MONA

#include <monapi.h>
#include <servers/screen.h>
#include "mona_shell.h"

using namespace MonAPI;
void scheme_on_input_char(char c);

int mona_shell_init()
{
    if (!monapi_register_to_server(ID_KEYBOARD_SERVER, 1))
    {
        printf("register to keyboard server failed\n");
        return -1;
    }

    MessageInfo msg;
    uint32_t targetID = Message::lookupMainThread("SCREEN.EX5");
    if (targetID == THREAD_UNKNOWN || Message::sendReceive(&msg, targetID, MSG_SCREEN_GET_STREAM_HANDLE)) {
        printf("SCREEN.EX5 not found\n");
        return -1;
    }
    uint32_t screenHandle = msg.arg2;


    monapi_register_to_server(ID_PROCESS_SERVER, 0);
    return 0;

}

int mona_shell_fini()
{
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    return 0;
}

void mona_shell_on_key_down(int keycode, int modifiers)
{
    switch(keycode) {
    case(Keys::A):case(Keys::B):case(Keys::C):case(Keys::D):
    case(Keys::E):case(Keys::F):case(Keys::G):case(Keys::H):
    case(Keys::I):case(Keys::J):case(Keys::K):case(Keys::L):
    case(Keys::M):case(Keys::N):case(Keys::O):case(Keys::P):
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
        if (keycode == Keys::H && modifiers & KEY_MODIFIER_CTRL)
        {
// backspace
            break;
        }
        else if (keycode == Keys::P && modifiers & KEY_MODIFIER_CTRL)
        {
// history
            break;
        }

        KeyInfo key;
        key.keycode = keycode;
        key.modifiers = modifiers;
        char c = Keys::ToChar(key);
        _printf("%c", c);
        scheme_on_input_char(Keys::ToChar(key));
        break;
    case(Keys::Enter):
        _printf("\n");
        scheme_on_input_char('\n');
        break;

    case(Keys::Back):
//back
        break;
    default:
        break;
    }
}


#else
int mona_shell_init()
{
}

int mona_shell_fini()
{
}

#endif
