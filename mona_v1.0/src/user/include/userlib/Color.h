#ifndef _MONA_USERLIB_COLOR_
#define _MONA_USERLIB_COLOR_

#include <types.h>

/*----------------------------------------------------------------------
    Color
----------------------------------------------------------------------*/
class Color {

  public:

    inline static word bpp24to565(dword color) {
        word result = (word)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
        return result;
    }

    inline static dword rgb(byte r, byte g, byte b) {
        return b | g  <<  8 | r  << 16;
    }
};
#endif
