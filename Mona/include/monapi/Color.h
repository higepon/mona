#ifndef _MONA_USERLIB_COLOR_
#define _MONA_USERLIB_COLOR_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Color
----------------------------------------------------------------------*/
class Color {

  public:

    inline static word bpp24to565(dword color) {
        word result = (word)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
        return result;
    }

    inline static word bpp24to565(byte r, byte g, byte b) {
        word result = (word)(((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | (b >> 3));
        return result;
    }

    inline static word bpp24to565(byte* rgb) {
        word result = (word)(((rgb[2] << 8) & 0xF800) | ((rgb[1] << 3) & 0x07E0) | (rgb[0] >> 3));
        return result;
    }

    inline static dword rgb(byte r, byte g, byte b) {
        return b | g  <<  8 | r  << 16;
    }
};

}

#endif
