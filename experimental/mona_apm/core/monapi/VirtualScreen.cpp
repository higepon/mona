/*----------------------------------------------------------------------
    VirtualScreen
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

VirtualScreen::VirtualScreen() : Screen() {

    int x = getWidth();
    int y = getHeight();
    vram_ = new byte[x * y * bpp_ / 8];
    if (vram_ == NULL) printf("vitual vram error\n");
}

VirtualScreen::VirtualScreen(dword vramsize) : Screen() {

    vram_ = new byte[vramsize];
    if (vram_ == NULL) printf("vitual vram error\n");
}


VirtualScreen::~VirtualScreen() {

    if (vram_) {
        delete vram_;
    }
}

}
