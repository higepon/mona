#ifndef _MONAPI_COLOR_
#define _MONAPI_COLOR_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Color
----------------------------------------------------------------------*/
class Color {

  public:

    inline static uint16_t bpp24to565(uint32_t color) {
        uint16_t result = (uint16_t)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
        return result;
    }

    inline static uint16_t bpp24to565(uint8_t r, uint8_t g, uint8_t b) {
        uint16_t result = (uint16_t)(((r << 8) & 0xF800) | ((g << 3) & 0x07E0) | (b >> 3));
        return result;
    }

    inline static uint16_t bpp24to565(uint8_t* rgb) {
        uint16_t result = (uint16_t)(((rgb[2] << 8) & 0xF800) | ((rgb[1] << 3) & 0x07E0) | (rgb[0] >> 3));
        return result;
    }

    inline static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
        return b | g  <<  8 | r  << 16;
    }
};

}

#endif
