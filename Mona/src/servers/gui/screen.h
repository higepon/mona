// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_SCREEN_H__
#define __GUISERVER_SCREEN_H__

#include <monapi.h>
#include "image.h"

extern MonAPI::Screen* GetDefaultScreen();
extern void SetMouseCursor(bool enabled);
extern void DrawImage(ImageInfo* img, int spx, int spy, int ix, int iy, int iw, int ih, int transparent);

#endif  // __GUISERVER_SCREEN_H__
