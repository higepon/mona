#ifndef _MONA_KEYS_
#define _MONA_KEYS_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Keys
----------------------------------------------------------------------*/
class Keys
{
public:
    enum
    {
        Back    = 8 ,
        Enter   = 13,
        Space   = 32,
        Left    = 37,
        Up      = 38,
        Right   = 39,
        Down    = 40,
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
        Decimal = 110,
        Divide  = 111,
        OemPlus = 187,
        OemMinus= 189
    };
};

}

#endif
