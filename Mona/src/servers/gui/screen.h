// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_SCREEN_H__
#define __GUISERVER_SCREEN_H__

#include <monapi.h>

extern MonAPI::Screen* GetDefaultScreen();
extern void SetMouseCursor(bool enabled);
extern void DrawImage(guiserver_bitmap* img, int spx = 0, int spy = 0, int ix = -1, int iy = -1, int iw = -1, int ih = -1, int transparent = -1);

#endif  // __GUISERVER_SCREEN_H__
