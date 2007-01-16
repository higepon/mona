/*----------------------------------------------------------------------
    VirtualScreen
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

VirtualScreen::VirtualScreen() : Screen() {

    int x = getWidth();
    int y = getHeight();
    vram_ = new uint8_t[x * y * bpp_ / 8];
    if (vram_ == NULL) printf("vitual vram error\n");
}

VirtualScreen::VirtualScreen(uint32_t vramsize) : Screen() {

    vram_ = new uint8_t[vramsize];
    if (vram_ == NULL) printf("vitual vram error\n");
}


VirtualScreen::~VirtualScreen() {

    if (vram_) {
        delete vram_;
    }
}

}
