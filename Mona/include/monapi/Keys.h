#ifndef _MONA_KEYS_
#define _MONA_KEYS_

#include <sys/types.h>

#define KEY_MODIFIER_DOWN  0x01
#define KEY_MODIFIER_UP    0x02
#define KEY_MODIFIER_SHIFT 0x04
#define KEY_MODIFIER_ALT   0x08
#define KEY_MODIFIER_CTRL  0x10
#define KEY_MODIFIER_WIN   0x20
#define KEY_MODIFIER_MENU  0x40
#define KEY_MODIFIER_CHAR  0x80

namespace MonAPI {

/*----------------------------------------------------------------------
    Keys
----------------------------------------------------------------------*/
class Keys{
  public:

    enum{
        US102,
        JP109,
    };

    enum{
        Back    = 8 ,
        Tab     = 9,
        Enter   = 13,
        ShiftKey= 16,
        ControlKey= 17,
        Menu    = 18,
        Pause   = 19,
        CapsLock= 20,
        KanaMode= 21,
        KanjiMode= 25,
        Escape  = 27,
        IMEConvert= 28,
        IMENonconvert= 29,
        IMEAceept= 30,
        IMEModeChange= 31,
        Space   = 32,
        PageUp  = 33,
        PageDown= 34,
        End     = 35,
        Home    = 36,
        Left    = 37,
        Up      = 38,
        Right   = 39,
        Down    = 40,
        PrintScreen = 44,
        Insert  = 45,
        Delete  = 46,
        D0      = 48,
        D1      = 49,
        D2      = 50,
        D3      = 51,
        D4      = 52,
        D5      = 53,
        D6      = 54,
        D7      = 55,
        D8      = 56,
        D9      = 57,
        A       = 65,
        B       = 66,
        C       = 67,
        D       = 68,
        E       = 69,
        F       = 70,
        G       = 71,
        H       = 72,
        I       = 73,
        J       = 74,
        K       = 75,
        L       = 76,
        M       = 77,
        N       = 78,
        O       = 79,
        P       = 80,
        Q       = 81,
        R       = 82,
        S       = 83,
        T       = 84,
        U       = 85,
        V       = 86,
        W       = 87,
        X       = 88,
        Y       = 89,
        Z       = 90,
        LWin    = 91,
        RWin    = 92,
        Apps    = 93,
        NumPad0 = 96,
        NumPad1 = 97,
        NumPad2 = 98,
        NumPad3 = 99,
        NumPad4 = 100,
        NumPad5 = 101,
        NumPad6 = 102,
        NumPad7 = 103,
        NumPad8 = 104,
        NumPad9 = 105,
        Multiply= 106,
        Add     = 107,
        Separator= 108,
        Subtract= 109,
        Decimal = 110,
        Divide  = 111,
        F1      = 112,
        F2      = 113,
        F3      = 114,
        F4      = 115,
        F5      = 116,
        F6      = 117,
        F7      = 118,
        F8      = 119,
        F9      = 120,
        F10     = 121,
        F11     = 122,
        F12     = 123,
        NumLock = 144,
        Scroll  = 145,
        LShiftKey = 160,
        RShiftKey = 161,
        LControlKey = 162,
        RControlKey = 163,
        LMenu       = 164,
        RMenu       = 165,
        OemSemicolon = 186,
        Oemplus  = 187,
        Oemcomma = 188,
        OemMinus = 189,
        OemPeriod= 190,
        OemQuestion= 191,
        Oemtilde = 192,
        OemOpenBrackets= 219,
        OemPipe  = 220,
        OemCloseBrackets= 221,
        OemQuotes= 222,
        OemBackslash= 226,

    };
    static const int mapUS102[128];
    static const int mapUS102E0[128];
    static const int mapJP109[128];
    static const int mapJP109E0[128];
    static const char keyTopMapUS102[256];
    static const char keyTopMapUS102S[256];
    static const char keyTopMapJP109[256];
    static const char keyTopMapJP109S[256];

    static char ToChar(KeyInfo keyInfo);
    static bool IsToChar(KeyInfo keyInfo);
    static char ToChar(KeyInfo keyInfo, char *map, char *maps);
    static bool IsToChar(KeyInfo keyInfo, char *map, char *maps);
};

}

#endif
